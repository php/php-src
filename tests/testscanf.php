<?php


function print_value($val,$postfix="<br>") {
	if (is_array($val)) {
		for ($i = 0;$i< count($val);$i++) {
			echo $val[$i] . $postfix;
		}
	} else {
		echo $val . $postfix;
	}
}

function do_sscanf($string, $format) {
	$s = "sscanf(\"" . $string . ",\"" . $format ."\").";
	echo "$s<br>";
	$s = str_repeat("-", strlen($s));
	echo "$s<br>";
	$output = sscanf($string,$format);
	echo "Result : ";
	print_value( $output );
	echo "$s<br><br>";
}


function run_sscanf_test_cases($filename="scan_cases")
{

	echo "<h3><em><br>Running Test Cases from $filename<br></em></h3>"; 
	$arr = file($filename);
	for ($i=0;$i < count($arr);$i++) {
		$line_arr = explode("|",$arr[$i]);
		
		$format = $line_arr[0];
		$string = $line_arr[1];
		if (count($arr) > 2) {
			$comment = $line_arr[2];
		} else {
			$comment = "";
		}
		if ( empty($format) || empty($string) ) {
			continue;
		}
		print("<h4>** Case : $comment ******************************</h4>");
		do_sscanf($string,$format);
	}
}

function simple_tests() {
	echo "Testing sscanf with standard ANSI syntax (values returned by
reference)-<br>";
	$decimal = -1;
	$string  = "";
	$hex	 = 0;
	$float	 = 0.0;	
	$octal	 = 0.0;
	$int	 = -1;
				
	echo "<h3><em><br>Simple Test<br></em></h3>"; 
	echo "sscanf('10','%d',&\$decimal) <br>";
	echo "<br>BEFORE : <br> decimal = $decimal.";
	$foo = sscanf("10","%d",&$decimal);
	echo "<br>AFTER  : <br> decimal = $decimal <br>";


	echo "<h3><em><br>Simple Test 2<br></em></h3>"; 
	echo "sscanf(\"ghost 0xface\",\"%s %x\",&\$string, &\$int)<br>";
	echo "<br>BEFORE : <br> string = $string, int = $int<br>";
	$foo = sscanf("ghost 0xface","%s %x",&$string, &$int);
	echo "<br>AFTER  : <br> string = $string, int = $int<br>";
	echo " sscan reports : ";
	print_value( $foo,"");
	echo " conversions <br>";

	echo "<h3><em><br>Multiple specifiers<br></em></h3>"; 
	echo "sscanf(\"jabberwocky 1024 0xFF 1.024 644 10\",
			\"%s %d  %x %f %o %i\",
			&\$string,&\$decimal,&\$hex,&\$float,&\$octal,&\$int);<br>";
	echo "<br>BEFORE : <br>";
	echo "Decimal = $decimal, String = $string, Hex = $hex<br>";
	echo "Octal = $octal , Float = $float, Int = $int<br>"; 
	$foo = sscanf(	"jabberwocky 1024 0xFF 1.024 644 10",
			"%s %d  %x %f %o %i",
			&$string,&$decimal,&$hex,&$float,&$octal,&$int);
	echo "<br>AFTER :<br>";
	echo "decimal = $decimal, string = $string, hex = $hex<br>";
	echo "octal = $octal , float = $float, int = $int<br>"; 
				
	echo " sscan reports : ";
	print_value( $foo,"");
	echo " conversions <br>";
	echo "----------------------------------------<br>";
}



?>
<html>
	<head>
		<title>Test of sscanf()</title>
	</head>
	<body>
		<strong><h1>Testing sscanf() support in PHP</h1></strong><br>	
		<?php
			if (!function_exists('sscanf')) {
				echo "<strong>I'm sorry but sscanf() does not exist !i</strong><br>";
			} else {
				simple_tests();
				run_sscanf_test_cases(); 
			}
		?>
	</body>	
</html>
