/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xmlwriter_open_uri, 0, 1, XMLWriter, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xmlwriter_open_memory, 0, 0, XMLWriter, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_set_indent, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, indent, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_set_indent_string, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, indentString, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_comment, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_comment arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_attribute, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_attribute arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_attribute_ns, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_attribute_ns, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_start_element arginfo_xmlwriter_start_attribute

#define arginfo_xmlwriter_end_element arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_full_end_element arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_element_ns, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_element, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_element_ns, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_pi, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_pi arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_pi, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_start_cdata arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_end_cdata arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_cdata, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_text arginfo_xmlwriter_write_cdata

#define arginfo_xmlwriter_write_raw arginfo_xmlwriter_write_cdata

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlwriter_start_document, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, standalone, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_document arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_write_comment arginfo_xmlwriter_write_cdata

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_dtd, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_dtd arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_dtd, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, subset, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_dtd_element, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_dtd_element arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_dtd_element, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_start_dtd_attlist arginfo_xmlwriter_start_attribute

#define arginfo_xmlwriter_end_dtd_attlist arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_write_dtd_attlist arginfo_xmlwriter_write_dtd_element

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_dtd_entity, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isparam, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_dtd_entity arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_dtd_entity, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isparam, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ndataid, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_output_memory, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, flush, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xmlwriter_flush, 0, 1, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_OBJ_INFO(0, xmlwriter, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, empty, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_openUri, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_openMemory, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_setIndent, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, indent, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_setIdentString, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, indentString, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startComment arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_endComment arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startAttribute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endAttribute arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeAttribute, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startAttributeNs, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeAttributeNs, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startElement arginfo_class_XMLWriter_startAttribute

#define arginfo_class_XMLWriter_endElement arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_fullEndElement arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startElementNs, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeElement, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeElementNs, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startPi, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endPi arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writePi, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startCdata arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_endCdata arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeCdata, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_text arginfo_class_XMLWriter_writeCdata

#define arginfo_class_XMLWriter_writeRaw arginfo_class_XMLWriter_writeCdata

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDocument, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, standalone, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDocument arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_writeComment arginfo_class_XMLWriter_writeCdata

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDtd, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtd arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeDtd, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, subset, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDtdElement, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtdElement arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeDtdElement, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startDtdAttlist arginfo_class_XMLWriter_startAttribute

#define arginfo_class_XMLWriter_endDtdAttlist arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_writeDtdAttlist arginfo_class_XMLWriter_writeDtdElement

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDtdEntity, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isparam, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtdEntity arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeDtdEntity, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isparam, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ndataid, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_outputMemory, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flush, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_XMLWriter_flush, 0, 0, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, empty, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
