#!/usr/bin/perl

$in_file = shift || die "Give .in file as first parameter";
$srcdir = shift || die "Give source dir as second parameter";
@extensions = @ARGV;

open(IN, $in_file) or die "Could not open .in file $in_file";
while (<IN>) {
	if (/\@EXT_INCLUDE_CODE\@/) {
		foreach $ext (@extensions) {
			$hdr_file = "ext/$ext/php3_${ext}.h";
			if (-f "$srcdir/$hdr_file") {
				print "#include \"ext/${ext}/php3_${ext}.h\"\n";
			}
		}
	} elsif (/\@EXT_MODULE_PTRS\@/) {
		foreach $ext (@extensions) {
			print "	phpext_${ext}_ptr,\n";
		}
	} else {
		print;
	}
}
close(IN);
