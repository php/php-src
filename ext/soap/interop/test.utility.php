<?php

function timestamp_to_soap_datetime($t) {
  return date('Y-m-d\TH:i:sO',$t);
}

function soap_datetime_to_timestamp($t) {
	/* Ignore Microsecconds */
  $iso8601 = '(-?[0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})(\.[0-9]*)?(Z|[+\-][0-9]{4}|[+\-][0-9]{2}:[0-9]{2})?';
  if (!is_int($t)) {
	  if (!ereg($iso8601,$t,$r)) {
  	  return false;
	  }
  	$t = gmmktime($r[4],$r[5],$r[6],$r[2],$r[3],$r[1]);
	  if (!empty($r[8]) && $r[8] != 'Z') {
		  $op = substr($r[8],0,1);
  		$h = substr($r[8],1,2);
	  	if (strstr($r[8],':')) {
	  	  $m = substr($r[8],4,2);
		  } else {
  		  $m = substr($r[8],3,2);
		  }
  		$t += (($op == "-"?1:-1) * $h * 60 + $m) * 60;
		}
  }
  return $t;
}

function date_compare($f1,$f2)
{
	return soap_datetime_to_timestamp($f1) == soap_datetime_to_timestamp($f2);
}

function hex_compare($f1, $f2)
{
  return strcasecmp($f1,$f2) == 0;
}

function number_compare($f1, $f2)
{
    # figure out which has the least fractional digits
    preg_match('/.*?\.(.*)/',$f1,$m1);
    preg_match('/.*?\.(.*)/',$f2,$m2);
    #print_r($m1);
    # always use at least 2 digits of precision
    $d = max(min(strlen(count($m1)?$m1[1]:'0'),strlen(count($m2)?$m2[1]:'0')),2);
    $f1 = round($f1, $d);
    $f2 = round($f2, $d);
    return $f1 == $f2;
//    return bccomp($f1, $f2, $d) == 0;
}

function boolean_compare($f1, $f2)
{
    if (($f1 == 'true' || $f1 === TRUE || $f1 != 0) &&
        ($f2 == 'true' || $f2 === TRUE || $f2 != 0)) return TRUE;
    if (($f1 == 'false' || $f1 === FALSE || $f1 == 0) &&
        ($f2 == 'false' || $f2 === FALSE || $f2 == 0)) return TRUE;
    return FALSE;
}

function string_compare($e1, $e2)
{
    if (is_numeric($e1) && is_numeric($e2)) {
        return number_compare($e1, $e2);
    }
    # handle dateTime comparison
    $e1_type = gettype($e1);
    $e2_type = gettype($e2);
    $ok = FALSE;
    if ($e1_type == "string") {
//        $dt = new SOAP_Type_dateTime();
//        $ok = $dt->compare($e1, $e2) == 0;
        $oj = false;
    }
    return $ok || $e1 == $e2 || strcasecmp(trim($e1), trim($e2)) == 0;
}

function array_compare(&$ar1, &$ar2) {
  if (gettype($ar1) != 'array' || gettype($ar2) != 'array') return FALSE;
  if (count($ar1) != count($ar2)) return FALSE;
  foreach ($ar1 as $k => $v) {
    if (!array_key_exists($k,$ar2)) return FALSE;
    if (!compare($v,$ar2[$k])) return FALSE;
  }
  return TRUE;
}

function object_compare(&$obj1, &$obj2) {
  if (gettype($obj1) != 'object' || gettype($obj2) != 'object') return FALSE;
//  if (class_name(obj1) != class_name(obj2)) return FALSE;
  $ar1 = (array)$obj1;
  $ar2 = (array)$obj2;
  return array_compare($ar1,$ar2);
}

function compare(&$x,&$y) {
  $ok = 0;
  $x_type = gettype($x);
  $y_type = gettype($y);
  if ($x_type == $y_type) {
    if ($x_type == "array") {
      $ok = array_compare($x, $y);
    } else if ($x_type == "object") {
      $ok = object_compare($x, $y);
    } else if ($x_type == "double") {
      $ok = number_compare($x, $y);
//    } else if ($x_type == 'boolean') {
//      $ok = boolean_compare($x, $y);
    } else {
    	$ok = ($x == $y);
//      $ok = string_compare($expect, $result);
    }
  }
  return $ok;
}


function parseMessage($msg)
{
    # strip line endings
    #$msg = preg_replace('/\r|\n/', ' ', $msg);
    $response = new SOAP_Parser($msg);
    if ($response->fault) {
        return $response->fault->getFault();
    }
    $return = $response->getResponse();
    $v = $response->decode($return);
    if (gettype($v) == 'array' && count($v)==1) {
        return array_shift($v);
    }
    return $v;
}

function var_dump_str($var) {
  ob_start();
  var_dump($var);
  $res = ob_get_contents();
  ob_end_clean();
  return $res;
}

?>