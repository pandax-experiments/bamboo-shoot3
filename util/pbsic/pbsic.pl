#   Copyright 2017 Xun Chen (xun.revive@gmail.com)
#
#   This script is part of bamboo-shoot3.
#
#   bamboo-shoot3 is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   bamboo-shoot3 is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with bamboo-shoot3.  If not, see <http://www.gnu.org/licenses/>.

#!/usr/bin/env perl

use strict;

my ($fin, $fout);
if ($#ARGV<0) {
  open ($fin, "<&STDIN");
  open ($fout, ">&STDOUT");
} elsif ($#ARGV == 0) {
  my $fin_name = shift;
  open ($fin, "<$fin_name") or die "Cannot open $fin_name for read: $!";
  open ($fout, ">&STDOUT");
} elsif ($#ARGV == 1) {
  my $fin_name = shift;
  my $fout_name = shift;
  open ($fin, "<$fin_name") or die "Cannot open $fin_name for read: $!";
  open ($fout, ">$fout_name") or die "Cannot open $fout_name for write: $!";
} else {
  die "Usage: $0 [input_file output_file]\n";
}

my $res = parse_input($fin);

for my $struct (@{$res}) {
  if ($struct->{type} eq 'struct' || $struct->{type} eq 'tuple') {
    print $fout 'struct ', $struct->{name}, ";\n";
  }
}
print $fout "\n";

for my $struct (@{$res}) {
  print $fout make_class($struct);
  print $fout "\n" unless $struct == ${$res}[$#{$res}];
}

sub parse_input {
  my $struct_start = 0;
  my $structs;
  my $current_name = '';
  while(<$fin>) {
    chomp;
    next if (m/^\s*\/\//);
    if (!$struct_start) {
      if ( m/(tuple|struct)\s+([\w\d]+)\s+\{/) {
        push @{$structs}, {'name' => $2, 'type' => $1};
        $current_name = $2;
        $struct_start = 1;
      } elsif ( m/enum\s+([\w\d]+)(\s+:?\s+([\w\d]+))?\s+\{/ ){
        $struct_start = 1;
        my $type = "uint32";
        $type = $3 if defined $3;
        push @{$structs}, {'name' => $1, 'type' => "enum : ".$type};
        $current_name = $1;
      }
      next;
    } elsif (m/\};/) {
      $struct_start = 0;
    } elsif (m/([\w\d]+)\s*=\s*(\d+);/) {
      my $name = $1;
      my $tag = $2;
      m/^\s*(.*)\s+$name/;
      my $type = $1;
      push @{$structs->[$#{$structs}]->{'var'}}, {'name' => $name,
                                                  'tag' => $tag,
                                                  'type' => $type};
    } elsif (m/([\w\d]+);/) {
      my $name = $1;
      m/^\s*(.*)\s+$name/;
      my $type = $1;
      push @{$structs->[$#{$structs}]->{'var'}}, {'name' => $name,
                                                  'type' => $type};
    } elsif (m/^\s*(.*\,.*)$/){
      push @{$structs->[$#{$structs}]->{'var'}}, {'name' => $1};
    }
  }
  return $structs;
}

sub make_class {
  my $struct = shift;
  my $class = "";
  my @names;
  my @tags;
  if ($struct->{type} eq 'struct' || $struct->{type} eq 'tuple') {
    $class .= "struct ". $struct->{name}. " {\n";
    for my $v (@{$struct->{var}}) {
      $class .= "    ".make_type($v->{type})." ".$v->{name}.";\n";
      push @names, $v->{name};
      push @tags, $v->{tag} if exists $v->{tag};
    }
    $class .= "\n";
    $class .= make_operator_equal($struct->{name}, \@names);
    $class .= make_tuple_code($struct->{name}, \@names) if $struct->{type} eq 'tuple';
    $class .= make_struct_member_tag($struct->{name}, \@names, \@tags) if $struct->{type} eq 'struct';
    $class .= "};\n";
  } elsif ($struct->{type} =~ /^enum/) {
    $class .= make_enum ($struct->{name}, $struct->{type}, $struct->{var});
  }
  $class;
}

sub make_type {
  my $raw_type = shift;
  my $new_type = $raw_type;
  $new_type =~ s/(int\d+)/$1_t/g;
  $new_type =~ s/\((\S+)\s=>\s(\S+)\)/std::map<$1, $2>/;
  $new_type =~ s/\[(\S+)\]/std::vector<$1>/;
  $new_type;
}

sub make_operator_equal {
  my $struct_name = shift;
  my $names = shift;
  my $text = "    bool operator==(const " . $struct_name. "& other) const\n    {\n";
  $text .= "        return ";
  for my $name (@{$names}) {
    $text .= "($name==other.$name)";
    if ($name ne $$names[$#$names]) {
      $text .= " && ";
    }
  }
  $text .= ";\n    }\n\n";
  $text;
}

sub make_tuple_code {
  my $struct_name = shift;
  my $names = shift;
  my $text = make_tuple_operator_less_than($struct_name, $names);
  $text .= make_tuple_member_declaration($struct_name, $names);
}

sub make_tuple_operator_less_than {
  my $struct_name = shift;
  my $names = shift;
  my $text = <<EOF;
    template <class Other>
    auto operator<(const Other& other) const
        -> typename std::enable_if<std::is_same<Other, $struct_name>::value, decltype(__TYPE__)>::type
    {
EOF
  my $type;
  for my $name (@{$names}) {
    $type .= "bool($name<other.$name)";
    $type .= ", " unless $name eq ${$names}[$#{$names}];
  }
  $text =~ s/__TYPE__/$type/;
  for my $name (@{$names}) {
    $text .= "        if ($name != other.$name) return $name < other.$name;\n";
  }
  $text .= "        return false;\n    }\n\n";
  $text;
}

sub make_tuple_member_declaration {
  my $struct_name = shift;
  my $names = shift;
  my $text = <<EOF;
    PBSS_TUPLE_MEMBERS(__MEMBER_DECLARATION__);
EOF
  my $members_text;
  for my $name (@{$names}) {
    $members_text .= "PBSS_TUPLE_MEMBER(&${struct_name}::${name})";
    $members_text .= ", " unless $name eq ${$names}[$#{$names}];
  }
  $text =~ s/__MEMBER_DECLARATION__/$members_text/;
  $text;
}

sub make_struct_member_tag {
  my $struct_name = shift;
  my $names = shift;
  my $tags = shift;
  my $text = <<EOF;
    PBSS_TAGGED_STRUCT(__MEMBER_DECLARATION__);
EOF
  my $members_text;
  for my $idx (0 .. $#{$names}) {
    $members_text .= "PBSS_TAG_MEMBER(${$tags}[$idx], &${struct_name}::${$names}[$idx])";
    $members_text .= ", " unless $idx eq $#{$names};
  }
  $text =~ s/__MEMBER_DECLARATION__/$members_text/;
  $text;
}

sub make_enum {
  my $enum_name = shift;
  my $type = shift;
  my $var = shift;
  $type =~ /:\s+(\S+)/;
  my $new_type = $1;
  $new_type =~ s/(int\d+)/$1_t/g;
  my $text = <<EOF
enum class $enum_name : $new_type {
    ${$var}[0]{name}
};
EOF
}
