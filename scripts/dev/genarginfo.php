#!/usr/bin/env php
<?php

$types = [ 
    'string'=>'IS_STRING',
    'int'=>'IS_LONG',
    'float'=>'IS_DOUBLE',
    'double'=>'IS_DOUBLE',
    'bool'=>'_IS_BOOL',
    'boolean'=>'_IS_BOOL',
    'array'=>'IS_ARRAY',
    'objects'=>'IS_OBJECT',
    'resource'=>'IS_RESOURCE',
    'callable'=>'IS_CALLABLE',
    'callback'=>'IS_CALLABLE'
];
           
$file_contents = file_get_contents($argv[1]);
$m = preg_match_all("~^\s*/\*\s+\{{3}\s+proto\s+(\S+)\s+(\w+)\s*\((.*?)\)~mU", $file_contents, $matches);
if($m) {
    $rets = $names = $param_strs = [];
    foreach($matches[1] as $mk => $mv) {
        $origs[]      = $matches[0][$mk];
        $rets[]       = $matches[1][$mk];
        $names[]      = $matches[2][$mk];
        $param_strs[] = $matches[3][$mk];
    }
    foreach($names as $k => $name) {
        $ret = $rets[$k];
        $param_str = $param_strs[$k];
        list(,$orig) = explode('proto ', $origs[$k]);
        list($count, $optional, $args) = parse_params($param_str); 
        $required = $count - $optional;
        echo "/* ".trim($orig)." */\n";
        if(!empty($types[$ret])) {
            echo "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_{$name}, 0, {$required}, {$types[$ret]}, 0, 0)\n";
        } else {
            echo "ZEND_BEGIN_ARG_INFO_EX(arginfo_{$name}, 0, 0, {$required})\n";
        }
        foreach($args as $arg) {
            list($type, $name) = explode(' ', trim($arg), 2);
            list($name) = explode('|', trim($name), 2); // No |'s in the names - choose the first
            $name = trim($name);
            if ($ref = ($name[0] == "&")) {
                $name = trim(substr($name, 1));
            }
            list($name, $default) = explode("=", $name, 2);
            $name = trim($name);
            $allow_null = (int) (trim($default) == "null");
            $ref = (int) $ref;
            $type=trim($type);
            if(!empty($types[$type])) {
                echo "\tZEND_ARG_TYPE_INFO($ref, {$name}, {$types[$type]}, $allow_null)\n";
            } else {
                echo "\tZEND_ARG_INFO($ref, {$name})\n";
            }
        }
        echo "ZEND_END_ARG_INFO()\n\n";
    }
} else {
    echo "No function prototypes found in {$argv[1]}\n";
}

function parse_params($str) {
    $str = trim(strtolower($str));
    if(empty($str) || $str=='void') return [0,0,[]];
    $params = explode(',', $str);
    $optional = 0;
    $args = [];
    foreach($params as $i=>$p) {
        $p = trim($p);
        if($p[0]=='[') {
            $optional++; 
            $p = ltrim($p, '[ '); 
        } else if($i) {
            if(substr($params[$i-1],-1)=='[') $optional++;
        }
        $p = rtrim($p, ' ]['); 
        $args[$i] = $p;
    }
    return [$i+1,$optional, $args];
}
