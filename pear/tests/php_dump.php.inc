<?php
class PHP_Dump {
    var $_var;
    function PHP_Dump($var)
    {
        $this->_var = $var;
    }
    
    function toPHP()
    {
        return $this->_toUnknown($this->_var);
    }
    
    function _toUnknown($var, $indent = '    ')
    {
        switch (gettype($var)) {
            case 'array' :
                return $this->_toArray($var, $indent);
            case 'boolean' :
                return $this->_toBool($var, $indent);
            case 'double' :
            case 'integer' :
                return $this->_toNumber($var, $indent);
            case 'NULL' :
                return "{$indent}null";
            case 'string' :
                return $this->_toString($var, $indent);
        }
    }
    
    function _toString($var, $indent)
    {
        return $indent . '"' . addslashes($var) . '"';
    }
    
    function _toBool($var, $indent)
    {
        return $indent . ($var ? 'true' : 'false');
    }
    
    function _toNumber($var, $indent)
    {
        return $indent . $var;
    }
    
    function _toArray($var, $indent = '    ')
    {
        $ret = $indent . "array(\n";
        foreach ($var as $key => $value) {
            $ret .= $indent . ((is_int($key) || is_double($key)) ? $key : "'$key'") . " =>\n";
            $ret .= $this->_toUnknown($value, "$indent    ") . ",\n";
        }
        $ret .= $indent . ')';
        return $ret;
    }
}
?>