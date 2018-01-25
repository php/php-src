--TEST--
token_name()
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

echo token_name(T_INCLUDE), "\n";
echo token_name(T_INCLUDE_ONCE), "\n";
echo token_name(T_EVAL), "\n";
echo token_name(T_REQUIRE), "\n";
echo token_name(T_REQUIRE_ONCE), "\n";
echo token_name(T_LOGICAL_OR), "\n";
echo token_name(T_LOGICAL_XOR), "\n";
echo token_name(T_LOGICAL_AND), "\n";
echo token_name(T_PRINT), "\n";
echo token_name(T_PLUS_EQUAL), "\n";
echo token_name(T_MINUS_EQUAL), "\n";
echo token_name(T_MUL_EQUAL), "\n";
echo token_name(T_DIV_EQUAL), "\n";
echo token_name(T_CONCAT_EQUAL), "\n";
echo token_name(T_MOD_EQUAL), "\n";
echo token_name(T_AND_EQUAL), "\n";
echo token_name(T_OR_EQUAL), "\n";
echo token_name(T_XOR_EQUAL), "\n";
echo token_name(T_SL_EQUAL), "\n";
echo token_name(T_SR_EQUAL), "\n";
echo token_name(T_BOOLEAN_OR), "\n";
echo token_name(T_BOOLEAN_AND), "\n";
echo token_name(T_IS_EQUAL), "\n";
echo token_name(T_IS_NOT_EQUAL), "\n";
echo token_name(T_IS_IDENTICAL), "\n";
echo token_name(T_IS_NOT_IDENTICAL), "\n";
echo token_name(T_IS_SMALLER_OR_EQUAL), "\n";
echo token_name(T_IS_GREATER_OR_EQUAL), "\n";
echo token_name(T_SL), "\n";
echo token_name(T_SR), "\n";
echo token_name(T_INC), "\n";
echo token_name(T_DEC), "\n";
echo token_name(T_INT_CAST), "\n";
echo token_name(T_DOUBLE_CAST), "\n";
echo token_name(T_STRING_CAST), "\n";
echo token_name(T_ARRAY_CAST), "\n";
echo token_name(T_OBJECT_CAST), "\n";
echo token_name(T_BOOL_CAST), "\n";
echo token_name(T_UNSET_CAST), "\n";
echo token_name(T_NEW), "\n";
echo token_name(T_EXIT), "\n";
echo token_name(T_IF), "\n";
echo token_name(T_ELSEIF), "\n";
echo token_name(T_ELSE), "\n";
echo token_name(T_ENDIF), "\n";
echo token_name(T_LNUMBER), "\n";
echo token_name(T_DNUMBER), "\n";
echo token_name(T_STRING), "\n";
echo token_name(T_STRING_VARNAME), "\n";
echo token_name(T_VARIABLE), "\n";
echo token_name(T_NUM_STRING), "\n";
echo token_name(T_INLINE_HTML), "\n";
echo token_name(T_ENCAPSED_AND_WHITESPACE), "\n";
echo token_name(T_CONSTANT_ENCAPSED_STRING), "\n";
echo token_name(T_ECHO), "\n";
echo token_name(T_DO), "\n";
echo token_name(T_WHILE), "\n";
echo token_name(T_ENDWHILE), "\n";
echo token_name(T_FOR), "\n";
echo token_name(T_ENDFOR), "\n";
echo token_name(T_FOREACH), "\n";
echo token_name(T_ENDFOREACH), "\n";
echo token_name(T_DECLARE), "\n";
echo token_name(T_ENDDECLARE), "\n";
echo token_name(T_AS), "\n";
echo token_name(T_SWITCH), "\n";
echo token_name(T_ENDSWITCH), "\n";
echo token_name(T_CASE), "\n";
echo token_name(T_DEFAULT), "\n";
echo token_name(T_BREAK), "\n";
echo token_name(T_CONTINUE), "\n";
echo token_name(T_FUNCTION), "\n";
echo token_name(T_CONST), "\n";
echo token_name(T_RETURN), "\n";
echo token_name(T_USE), "\n";
echo token_name(T_GLOBAL), "\n";
echo token_name(T_STATIC), "\n";
echo token_name(T_VAR), "\n";
echo token_name(T_UNSET), "\n";
echo token_name(T_ISSET), "\n";
echo token_name(T_EMPTY), "\n";
echo token_name(T_CLASS), "\n";
echo token_name(T_EXTENDS), "\n";
echo token_name(T_INTERFACE), "\n";
echo token_name(T_IMPLEMENTS), "\n";
echo token_name(T_OBJECT_OPERATOR), "\n";
echo token_name(T_DOUBLE_ARROW), "\n";
echo token_name(T_LIST), "\n";
echo token_name(T_ARRAY), "\n";
echo token_name(T_CLASS_C), "\n";
echo token_name(T_FUNC_C), "\n";
echo token_name(T_METHOD_C), "\n";
echo token_name(T_LINE), "\n";
echo token_name(T_FILE), "\n";
echo token_name(T_COMMENT), "\n";
echo token_name(T_DOC_COMMENT), "\n";
echo token_name(T_OPEN_TAG), "\n";
echo token_name(T_OPEN_TAG_WITH_ECHO), "\n";
echo token_name(T_CLOSE_TAG), "\n";
echo token_name(T_WHITESPACE), "\n";
echo token_name(T_START_HEREDOC), "\n";
echo token_name(T_END_HEREDOC), "\n";
echo token_name(T_DOLLAR_OPEN_CURLY_BRACES), "\n";
echo token_name(T_CURLY_OPEN), "\n";
echo token_name(T_PAAMAYIM_NEKUDOTAYIM), "\n";
echo token_name(T_PAAMAYIM_NEKUDOTAYIM), "\n";
echo token_name(T_ABSTRACT), "\n";
echo token_name(T_CATCH), "\n";
echo token_name(T_FINAL), "\n";
echo token_name(T_INSTANCEOF), "\n";
echo token_name(T_PRIVATE), "\n";
echo token_name(T_PROTECTED), "\n";
echo token_name(T_PUBLIC), "\n";
echo token_name(T_THROW), "\n";
echo token_name(T_TRY), "\n";
echo token_name(T_CLONE), "\n";
echo token_name(T_HALT_COMPILER), "\n";

echo token_name(-1), "\n";
echo token_name(0x8000000F), "\n";
echo token_name("string"), "\n";
echo token_name(array()), "\n";

echo "Done\n";
?>
--EXPECTF--
T_INCLUDE
T_INCLUDE_ONCE
T_EVAL
T_REQUIRE
T_REQUIRE_ONCE
T_LOGICAL_OR
T_LOGICAL_XOR
T_LOGICAL_AND
T_PRINT
T_PLUS_EQUAL
T_MINUS_EQUAL
T_MUL_EQUAL
T_DIV_EQUAL
T_CONCAT_EQUAL
T_MOD_EQUAL
T_AND_EQUAL
T_OR_EQUAL
T_XOR_EQUAL
T_SL_EQUAL
T_SR_EQUAL
T_BOOLEAN_OR
T_BOOLEAN_AND
T_IS_EQUAL
T_IS_NOT_EQUAL
T_IS_IDENTICAL
T_IS_NOT_IDENTICAL
T_IS_SMALLER_OR_EQUAL
T_IS_GREATER_OR_EQUAL
T_SL
T_SR
T_INC
T_DEC
T_INT_CAST
T_DOUBLE_CAST
T_STRING_CAST
T_ARRAY_CAST
T_OBJECT_CAST
T_BOOL_CAST
T_UNSET_CAST
T_NEW
T_EXIT
T_IF
T_ELSEIF
T_ELSE
T_ENDIF
T_LNUMBER
T_DNUMBER
T_STRING
T_STRING_VARNAME
T_VARIABLE
T_NUM_STRING
T_INLINE_HTML
T_ENCAPSED_AND_WHITESPACE
T_CONSTANT_ENCAPSED_STRING
T_ECHO
T_DO
T_WHILE
T_ENDWHILE
T_FOR
T_ENDFOR
T_FOREACH
T_ENDFOREACH
T_DECLARE
T_ENDDECLARE
T_AS
T_SWITCH
T_ENDSWITCH
T_CASE
T_DEFAULT
T_BREAK
T_CONTINUE
T_FUNCTION
T_CONST
T_RETURN
T_USE
T_GLOBAL
T_STATIC
T_VAR
T_UNSET
T_ISSET
T_EMPTY
T_CLASS
T_EXTENDS
T_INTERFACE
T_IMPLEMENTS
T_OBJECT_OPERATOR
T_DOUBLE_ARROW
T_LIST
T_ARRAY
T_CLASS_C
T_FUNC_C
T_METHOD_C
T_LINE
T_FILE
T_COMMENT
T_DOC_COMMENT
T_OPEN_TAG
T_OPEN_TAG_WITH_ECHO
T_CLOSE_TAG
T_WHITESPACE
T_START_HEREDOC
T_END_HEREDOC
T_DOLLAR_OPEN_CURLY_BRACES
T_CURLY_OPEN
T_DOUBLE_COLON
T_DOUBLE_COLON
T_ABSTRACT
T_CATCH
T_FINAL
T_INSTANCEOF
T_PRIVATE
T_PROTECTED
T_PUBLIC
T_THROW
T_TRY
T_CLONE
T_HALT_COMPILER
UNKNOWN
UNKNOWN

Warning: token_name() expects parameter 1 to be integer, string given in %s on line %d


Warning: token_name() expects parameter 1 to be integer, array given in %s on line %d

Done
