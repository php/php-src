#!/usr/bin/perl
# script to generate Shift_JIS encoded Emoji to/from Unicode conversion table. 
# Rui Hirokawa <hirokawa@php.net>
#
# usage: mktbl.pl EmojiSources.txt
#
# Unicoe;DoCoMo;KDDI;SoftBank

@docomo = ();
@kddi = ();
@softbank = ();

@to_docomo = ();
@to_kddi = ();
@to_sb = ();

$fname = "emoji2uni.h";
open(OUT,">$fname") or die $!;

sub sjis2code {
  my @c = unpack("C*", pack("H4", $_[0]));

  # Shift_JIS -> JIS
  $c[0] = (($c[0]-($c[0]<160?112:176))<<1)-($c[1]<159?1:0);
  $c[1] -= ($c[1]<159?($c[1]>127?32:31):126); 

  $s = ($c[0] - 0x21)*94 + $c[1]-0x21;

  return $s;
}

sub show_code {
    my @c = @_;
    $s = "\t";
    for ($i=0; $i<=$#c; $i++) {#
	if ($c[$i]) {
	    @v = split(' ',$c[$i]);
	    $s .= "0x$v[0], \t";
	    if ($#v > 0) {
		print "$i $v[0] $v[1]\n";
	    }
	} else {
	    $s .= "0x0000, \t";
	}
	if ($i % 4 == 3) {
	    $s .= "\n\t";
	}
    }
    return $s;
}

while(<>) {
    if ($_ =~ /^\d+/) {
	@v = split(/;/,$_);
	if ($v[1] =~ /[\dA-F]+/) {
	    $code = &sjis2code($v[1]);
	    $docomo{$code} = $v[0];
	    $to_docomo{$v[0]} = $code;
	}
	if ($v[2] =~ /[\dA-F]+/) {
	    $code = &sjis2code($v[2]);
	    $kddi{$code} = $v[0];
	    $to_kddi{$v[0]} = $code;
	}
	if ($v[3] =~ /[\dA-F]+/) {
	    $code = &sjis2code($v[3]);
	    $softbank{$code} = $v[0];
	    $to_sb{$v[0]} = $code;
	}
    }
}

print "DoCoMo\n";

$docomo_min = 10434;
$docomo_max = 10434+281;
@docomo_v = ();

foreach $key (sort {hex($a) <=> hex($b)} keys(%docomo)) {
    $s = $key;
    $pos = $s % 94;
    $ku = ($s - $pos)/94;
    $v = $key - $docomo_min;
    #print "$ku:$pos - ". $v ."=> $docomo{$key}\n";
    $docomo_v[$key-$docomo_min] = $docomo{$key};
}

$to_docomo_min = 10434;

$to_docomo_min1 = 0x0023;
$to_docomo_max1 = 0x00AE;
$to_docomo_min2 = 0x203C;
$to_docomo_max2 = 0x3299;
$to_docomo_min3 = 0x1F17F;
$to_docomo_max3 = 0x1F6BB;

@r_docomo1_key = ();
@r_docomo1_val = ();

@r_docomo2_key = ();
@r_docomo2_val = ();

@r_docomo3_key = ();
@r_docomo3_val = ();


foreach $key (sort {hex($a) <=> hex($b)} keys(%to_docomo)) {
    $s = $to_docomo{$key};

    $pos = $s % 94;
    $ku = ($s - $pos)/94;
    $v = $to_docomo{$key} - $to_docomo_min;
    $h = sprintf("%x",$s);
    #print "$ku:$pos = $h ($v) <= $key\n";
    if (hex($key) <= $to_docomo_max1) {
	push(@r_docomo1_key, $key);
	push(@r_docomo1_val, sprintf("%x", $to_docomo{$key}));
    } elsif (hex($key) <= $to_docomo_max2) {
	push(@r_docomo2_key, $key);
	push(@r_docomo2_val, $h);
    } elsif (hex($key) >= $to_docomo_max3) {
	push(@r_docomo3_key, $key);
	push(@r_docomo3_val, $h);
    }
}

push(@r_docomo1_key, 0x00);
push(@r_docomo1_val, 0x00);
push(@r_docomo2_key, 0x00);
push(@r_docomo2_val, 0x00);
push(@r_docomo3_key, 0x00);
push(@r_docomo3_val, 0x00);

print OUT "int mb_tbl_code2uni_docomo_min = $docomo_min;\n"; 
print OUT "int mb_tbl_code2uni_docomo_max = $docomo_max;\n\n"; 

print OUT "int mb_tbl_code2uni_docomo[] = {\n";
print OUT &show_code(@docomo_v);
print OUT "};\n\n";

print OUT "int mb_tbl_uni_docomo2code_min1 = $to_docomo_min1;\n"; 
print OUT "int mb_tbl_uni_docomo2code_max1 = $to_docomo_max1;\n"; 
print OUT "int mb_tbl_uni_docomo2code_min2 = $to_docomo_min2;\n"; 
print OUT "int mb_tbl_uni_docomo2code_max2 = $to_docomo_max2;\n"; 
print OUT "int mb_tbl_uni_docomo2code_min3 = $to_docomo_min3;\n"; 
print OUT "int mb_tbl_uni_docomo2code_max3 = $to_docomo_max3;\n\n"; 

#print "DOCOMO reverse 1\n";

print OUT "int mb_tbl_uni_docomo2code_key1[] = {\n";
print OUT &show_code(@r_docomo1_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_docomo2code_val1[] = {\n";
print OUT &show_code(@r_docomo1_val),"\n";
print OUT "};\n\n";

#print "DOCOMO reverse 2\n";

print OUT "int mb_tbl_uni_docomo2code_key2[] = {\n";
print OUT &show_code(@r_docomo2_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_docomo2code_val2[] = {\n";
print OUT &show_code(@r_docomo2_val),"\n";
print OUT "};\n\n";

print "DOCOMO reverse 3\n";

print OUT "int mb_tbl_uni_docomo2code_key3[] = {\n";
print OUT &show_code(@r_docomo3_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_docomo2code_val3[] = {\n";
print OUT &show_code(@r_docomo3_val),"\n";
print OUT "};\n\n";

#print "DOCOMO reverse end \n";

$kddi_min1 = 9400;
$kddi_max1 = 9400+264;
$kddi_min2 = 9400+564;
$kddi_max2 = 9400+939;

@kddi_v1 = ();
@kddi_v2 = ();

#print "KDDI\n";

foreach $key (sort {hex($a) <=> hex($b)} keys(%kddi)) {
    $s = $key;
    $pos = $s % 94;
    $ku = ($s - $pos)/94;
    $v = $key - $kddi_min1;
    $h = sprintf("%x",$key);
    #print "$ku:$pos :: $v ($h) => $kddi{$key}\n";

    if ($key <= $kddi_max1) {
	$kddi_v1[$key-$kddi_min1] = $kddi{$key};
    } elsif ($key <= $kddi_max2) {
	$kddi_v2[$key-$kddi_min2] = $kddi{$key};
    }
}

$to_kddi_min = 9660;
$to_kddi_min1 = 0x0030;
$to_kddi_max1 = 0x00AE;
$to_kddi_min2 = 0x2002;
$to_kddi_max2 = 0x3299;
$to_kddi_min3 = 0x1F004;
$to_kddi_max3 = 0x1F6C0;

@r_kddi1_key = (); @r_kddi1_val = ();
@r_kddi2_key = (); @r_kddi2_val = ();
@r_kddi3_key = (); @r_kddi3_val = ();

foreach $key (sort {hex($a) <=> hex($b)} keys(%to_kddi)) {
    $s = $to_kddi{$key};

    $pos = $s % 94;
    $ku = ($s - $pos)/94;
    $v = $to_kddi{$key} - $to_kddi_min;
    $h = sprintf("%x",$s);
    #print "$ku:$pos = $h ($v) <= $key\n";
    if (hex($key) <= $to_kddi_max1) {
	push(@r_kddi1_key, $key);
	push(@r_kddi1_val, $h);
    } elsif (hex($key) <= $to_kddi_max2) {
	push(@r_kddi2_key, $key);
	push(@r_kddi2_val, $h);
    } else {
	push(@r_kddi3_key, $key);
	push(@r_kddi3_val, $h);
    }
}

push(@r_kddi1_key, 0x00);
push(@r_kddi1_val, 0x00);
push(@r_kddi2_key, 0x00);
push(@r_kddi2_val, 0x00);
push(@r_kddi3_key, 0x00);
push(@r_kddi3_val, 0x00);

print OUT "int mb_tbl_code2uni_kddi1_min = $kddi_min1;\n"; 
print OUT "int mb_tbl_code2uni_kddi1_max = $kddi_max1;\n"; 
print OUT "int mb_tbl_code2uni_kddi2_min = $kddi_min2;\n"; 
print OUT "int mb_tbl_code2uni_kddi2_max = $kddi_max2;\n\n"; 

#print "KDDI 1\n";

print OUT "int mb_tbl_code2uni_kddi1[] = {\n";
print OUT &show_code(@kddi_v1);
print OUT "};\n\n";

#print "KDDI 2\n";

print OUT "int mb_tbl_code2uni_kddi2[] = {\n";
print OUT &show_code(@kddi_v2);
print OUT "};\n\n";

print OUT "int mb_tbl_uni_kddi2code_min1 = $to_kddi_min1;\n"; 
print OUT "int mb_tbl_uni_kddi2code_max1 = $to_kddi_max1;\n"; 
print OUT "int mb_tbl_uni_kddi2code_min2 = $to_kddi_min2;\n"; 
print OUT "int mb_tbl_uni_kddi2code_max2 = $to_kddi_max2;\n"; 
print OUT "int mb_tbl_uni_kddi2code_min3 = $to_kddi_min3;\n"; 
print OUT "int mb_tbl_uni_kddi2code_max3 = $to_kddi_max3;\n\n"; 

#print "KDDI reverse 1\n";

print OUT "int mb_tbl_uni_kddi2code_key1[] = {\n";
print OUT &show_code(@r_kddi1_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_kddi2code_val1[] = {\n";
print OUT &show_code(@r_kddi1_val),"\n";
print OUT "};\n\n";

#print "KDDI reverse 1\n";

print OUT "int mb_tbl_uni_kddi2code_key2[] = {\n";
print OUT &show_code(@r_kddi2_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_kddi2code_val2[] = {\n";
print OUT &show_code(@r_kddi2_val),"\n";
print OUT "};\n\n";

#print "KDDI reverse 3\n";

print OUT "int mb_tbl_uni_kddi2code_key3[] = {\n";
print OUT &show_code(@r_kddi3_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_kddi2code_val3[] = {\n";
print OUT &show_code(@r_kddi3_val),"\n";
print OUT "};\n\n";


$sb_min1 = 10153;
$sb_max1 = 10153+177;
$sb_min2 = 10153+376;
$sb_max2 = 10153+547;
$sb_min3 = 10153+752;
$sb_max3 = 10153+901;

@sb_v1 = ();
@sb_v2 = ();
@sb_v3 = ();

if (1) {
    print "SoftBank\n";
    
    foreach $key (sort {hex($a) <=> hex($b)} keys(%softbank)) {
	$s = $key;
	$pos = $s % 94;
	$ku = ($s - $pos)/94;
	$v = $key - $sb_min1;
	$h = sprintf("%x",$key);
	#print "$ku:$pos :: $v ($h) => $softbank{$key}\n";
	if ($key <= $sb_max1) {
	    $sb_v1[$key-$sb_min1] = $softbank{$key};
	} elsif ($key <= $sb_max2) {
	    $sb_v2[$key-$sb_min2] = $softbank{$key};
	} elsif ($key <= $sb_max3) {
	    $sb_v3[$key-$sb_min3] = $softbank{$key};
	}
    }
   
}

$to_sb_min = 10263;
$to_sb_min1 = 0x0023;
$to_sb_max1 = 0x00AE;
$to_sb_min2 = 0x2122;
$to_sb_max2 = 0x3299;
$to_sb_min3 = 0x1F004;
$to_sb_max3 = 0x1F6C0;

@r_sb1_key = (); @r_sb1_val = ();
@r_sb2_key = (); @r_sb2_val = ();
@r_sb3_key = (); @r_sb3_val = ();

foreach $key (sort {hex($a) <=> hex($b)} keys(%to_sb)) {
    $s = $to_sb{$key};

    $pos = $s % 94;
    $ku = ($s - $pos)/94;
    $v = $to_sb{$key} - $to_sb_min;
    $h = sprintf("%x",$s);
    #print "$ku:$pos = $h ($v) <= $key\n";
    if (hex($key) <= $to_sb_max1) {
	push(@r_sb1_key, $key);
	push(@r_sb1_val, $h);
    } elsif (hex($key) >= $to_sb_min2 && hex($key) <= $to_sb_max2) {
	push(@r_sb2_key, $key);
	push(@r_sb2_val, $h);
    } else {
	push(@r_sb3_key, $key);
	push(@r_sb3_val, $h);
    }
}

push(@r_sb1_key, 0x00);
push(@r_sb1_val, 0x00);
push(@r_sb2_key, 0x00);
push(@r_sb2_val, 0x00);
push(@r_sb3_key, 0x00);
push(@r_sb3_val, 0x00);


print OUT "int mb_tbl_code2uni_sb1_min = $sb_min1;\n"; 
print OUT "int mb_tbl_code2uni_sb1_max = $sb_max1;\n"; 
print OUT "int mb_tbl_code2uni_sb2_min = $sb_min2;\n"; 
print OUT "int mb_tbl_code2uni_sb2_max = $sb_max2;\n"; 
print OUT "int mb_tbl_code2uni_sb3_min = $sb_min3;\n"; 
print OUT "int mb_tbl_code2uni_sb3_max = $sb_max3;\n\n"; 

#print "SoftBank 1\n";

print OUT "int mb_tbl_code2uni_sb1[] = {\n";
print OUT &show_code(@sb_v1);
print OUT "};\n\n";

#print "SoftBank 2\n";

print OUT "int mb_tbl_code2uni_sb2[] = {\n";
print OUT &show_code(@sb_v2);
print OUT "};\n\n";

#print "SoftBank 3\n";

print OUT "int mb_tbl_code2uni_sb3[] = {\n";
print OUT &show_code(@sb_v3);
print OUT "};\n\n";

print OUT "int mb_tbl_uni_sb2code_min1 = $to_sb_min1;\n"; 
print OUT "int mb_tbl_uni_sb2code_max1 = $to_sb_max1;\n"; 
print OUT "int mb_tbl_uni_sb2code_min2 = $to_sb_min2;\n"; 
print OUT "int mb_tbl_uni_sb2code_max2 = $to_sb_max2;\n"; 
print OUT "int mb_tbl_uni_sb2code_min3 = $to_sb_min3;\n"; 
print OUT "int mb_tbl_uni_sb2code_max3 = $to_sb_max3;\n\n"; 

#print "SB reverse 1\n";

print OUT "int mb_tbl_uni_sb2code_key1[] = {\n";
print OUT &show_code(@r_sb1_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_sb2code_val1[] = {\n";
print OUT &show_code(@r_sb1_val),"\n";
print OUT "};\n\n";

#print "SB reverse 2\n";

print OUT "int mb_tbl_uni_sb2code_key2[] = {\n";
print OUT &show_code(@r_sb2_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_sb2code_val2[] = {\n";
print OUT &show_code(@r_sb2_val),"\n";
print OUT "};\n\n";

#print "SB reverse 3\n";

print OUT "int mb_tbl_uni_sb2code_key3[] = {\n";
print OUT &show_code(@r_sb3_key),"\n";
print OUT "};\n\n";
print OUT "int mb_tbl_uni_sb2code_val3[] = {\n";
print OUT &show_code(@r_sb3_val),"\n";
print OUT "};\n\n";


close(OUT);
