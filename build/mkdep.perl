#!/usr/bin/perl
#
# $Id$
#
# Created: Thu Aug 15 11:57:33 1996 too
# Last modified: Mon Dec 27 09:23:56 1999 too
#
# Copyright (c) 1996-1999 Tomi Ollila.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
# EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

die "Usage: mkdep CPP-command [CPP options] file1 [file2...]\n"
  if ($#ARGV < 1);

$cmdl = shift(@ARGV);

$cmdl = "$cmdl " . shift (@ARGV) while ($ARGV[0] =~ /^-[A-Z]/);
  
while ($file = shift(@ARGV))
{
    $file =~ s/\.o$/.c/;

    open(F, "$cmdl $file|");

    &parseout;

    close(F);
}


sub initinit
{
    %used = ();
    $of = $file;
    $of =~ s/\.c$/.lo/;
    $str = "$of:\t$file";
    $len = length $str;
}
	
sub initstr
{
    $str = "\t";
    $len = length $str;
}

sub parseout
{
    &initinit;
    while (<F>)
    {
	next unless (/^# [0-9]* "(.*\.h)"/);

	next if ($1 =~ /^\//);

	next if $used{$1};

        $used{$1} = 1;

	$nlen = length($1) + 1;

	if ($len + $nlen > 72)
	{
	    print $str, "\\\n";
	    &initstr;
	    $str = $str . $1;
	}
	else { $str = $str . " " . $1; }
		     
	$len += $nlen;	
		     
    }
    print $str, "\n";
}
