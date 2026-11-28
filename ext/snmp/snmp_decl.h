/* This is a generated file, edit snmp.stub.php instead.
 * Stub hash: d2c22c27a27f9b1c40c94409236d3068d1b8128a */

#ifndef ZEND_SNMP_DECL_d2c22c27a27f9b1c40c94409236d3068d1b8128a_H
#define ZEND_SNMP_DECL_d2c22c27a27f9b1c40c94409236d3068d1b8128a_H

typedef enum zend_enum_Snmp_Mib {
	ZEND_ENUM_Snmp_Mib_AllowUnderscores = 1,
	ZEND_ENUM_Snmp_Mib_CommentTerm = 2,
	ZEND_ENUM_Snmp_Mib_Replace = 3,
} zend_enum_Snmp_Mib;

typedef enum zend_enum_Snmp_OidOutput {
	ZEND_ENUM_Snmp_OidOutput_Suffix = 1,
	ZEND_ENUM_Snmp_OidOutput_Module = 2,
	ZEND_ENUM_Snmp_OidOutput_Full = 3,
	ZEND_ENUM_Snmp_OidOutput_Numeric = 4,
	ZEND_ENUM_Snmp_OidOutput_Ucd = 5,
	ZEND_ENUM_Snmp_OidOutput_None = 6,
} zend_enum_Snmp_OidOutput;

typedef enum zend_enum_Snmp_Output {
	ZEND_ENUM_Snmp_Output_NumericIndex = 1,
	ZEND_ENUM_Snmp_Output_EnumPrint = 2,
	ZEND_ENUM_Snmp_Output_EscapeQuotes = 3,
	ZEND_ENUM_Snmp_Output_QuickPrint = 4,
	ZEND_ENUM_Snmp_Output_NumericTimeticks = 5,
	ZEND_ENUM_Snmp_Output_HexText = 6,
	ZEND_ENUM_Snmp_Output_DontPrintUnits = 7,
	ZEND_ENUM_Snmp_Output_ExtendedIndex = 8,
} zend_enum_Snmp_Output;

typedef enum zend_enum_Snmp_StringOutput {
	ZEND_ENUM_Snmp_StringOutput_Guess = 1,
	ZEND_ENUM_Snmp_StringOutput_Ascii = 2,
	ZEND_ENUM_Snmp_StringOutput_Hex = 3,
} zend_enum_Snmp_StringOutput;

#endif /* ZEND_SNMP_DECL_d2c22c27a27f9b1c40c94409236d3068d1b8128a_H */
