<?php
require_once("SOAP/Parser.php");
require_once("SOAP/Value.php");

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
    return bccomp($f1, $f2, $d) == 0;
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
        $dt = new SOAP_Type_dateTime();
        $ok = $dt->compare($e1, $e2) == 0;
    }
    return $ok || $e1 == $e2 || strcasecmp(trim($e1), trim($e2)) == 0;
}

function array_compare(&$ar1, &$ar2)
{
    if (gettype($ar1) != 'array' || gettype($ar2) != 'array') return FALSE;
    # first a shallow diff
    if (count($ar1) != count($ar2)) return FALSE;
    $diff = array_diff($ar1, $ar2);
    if (count($diff) == 0) return TRUE;

    # diff failed, do a full check of the array
    foreach ($ar1 as $k => $v) {
        #print "comparing $v == $ar2[$k]\n";
        if (gettype($v) == "array") {
            if (!array_compare($v, $ar2[$k])) return FALSE;
        } else {
            if (!string_compare($v, $ar2[$k])) return FALSE;
        }
    }
    return TRUE;
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


?>