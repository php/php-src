/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a0ece6bc77b0a9811cb09a604b175e2295efc7a0 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xmlwriter_open_uri, 0, 1, XMLWriter, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xmlwriter_open_memory, 0, 0, XMLWriter, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_set_indent, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_set_indent_string, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, indentation, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_comment, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_comment arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_attribute, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_attribute arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_attribute, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_attribute_ns, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_attribute_ns, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_start_element arginfo_xmlwriter_start_attribute

#define arginfo_xmlwriter_end_element arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_full_end_element arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_start_element_ns arginfo_xmlwriter_start_attribute_ns

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_element, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_element_ns, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_pi, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_pi arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_pi, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_start_cdata arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_end_cdata arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_cdata, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_text arginfo_xmlwriter_write_cdata

#define arginfo_xmlwriter_write_raw arginfo_xmlwriter_write_cdata

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_document, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, version, IS_STRING, 1, "\"1.0\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, standalone, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_document arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_write_comment arginfo_xmlwriter_write_cdata

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_dtd, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_dtd arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_dtd, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_dtd_element, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_dtd_element arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_dtd_element, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_start_dtd_attlist arginfo_xmlwriter_start_attribute

#define arginfo_xmlwriter_end_dtd_attlist arginfo_xmlwriter_start_comment

#define arginfo_xmlwriter_write_dtd_attlist arginfo_xmlwriter_write_dtd_element

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_start_dtd_entity, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isParam, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_xmlwriter_end_dtd_entity arginfo_xmlwriter_start_comment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_write_dtd_entity, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, isParam, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, notationData, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlwriter_output_memory, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flush, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xmlwriter_flush, 0, 1, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_OBJ_INFO(0, writer, XMLWriter, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, empty, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_openUri, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_openMemory, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_setIndent, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_setIndentString, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, indentation, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startComment arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_endComment arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endAttribute arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startAttributeNs, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeAttributeNs, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startElement arginfo_class_XMLWriter_startAttribute

#define arginfo_class_XMLWriter_endElement arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_fullEndElement arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_startElementNs arginfo_class_XMLWriter_startAttributeNs

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeElement, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeElementNs, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startPi, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endPi arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writePi, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startCdata arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_endCdata arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeCdata, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_text arginfo_class_XMLWriter_writeCdata

#define arginfo_class_XMLWriter_writeRaw arginfo_class_XMLWriter_writeCdata

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startDocument, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, version, IS_STRING, 1, "\"1.0\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, standalone, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDocument arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_writeComment arginfo_class_XMLWriter_writeCdata

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startDtd, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtd arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeDtd, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startDtdElement, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtdElement arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeDtdElement, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startDtdAttlist arginfo_class_XMLWriter_startAttribute

#define arginfo_class_XMLWriter_endDtdAttlist arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_writeDtdAttlist arginfo_class_XMLWriter_writeDtdElement

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_startDtdEntity, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isParam, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtdEntity arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_writeDtdEntity, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, isParam, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, notationData, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_outputMemory, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flush, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLWriter_flush, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, empty, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(xmlwriter_open_uri);
ZEND_FUNCTION(xmlwriter_open_memory);
ZEND_FUNCTION(xmlwriter_set_indent);
ZEND_FUNCTION(xmlwriter_set_indent_string);
ZEND_FUNCTION(xmlwriter_start_comment);
ZEND_FUNCTION(xmlwriter_end_comment);
ZEND_FUNCTION(xmlwriter_start_attribute);
ZEND_FUNCTION(xmlwriter_end_attribute);
ZEND_FUNCTION(xmlwriter_write_attribute);
ZEND_FUNCTION(xmlwriter_start_attribute_ns);
ZEND_FUNCTION(xmlwriter_write_attribute_ns);
ZEND_FUNCTION(xmlwriter_start_element);
ZEND_FUNCTION(xmlwriter_end_element);
ZEND_FUNCTION(xmlwriter_full_end_element);
ZEND_FUNCTION(xmlwriter_start_element_ns);
ZEND_FUNCTION(xmlwriter_write_element);
ZEND_FUNCTION(xmlwriter_write_element_ns);
ZEND_FUNCTION(xmlwriter_start_pi);
ZEND_FUNCTION(xmlwriter_end_pi);
ZEND_FUNCTION(xmlwriter_write_pi);
ZEND_FUNCTION(xmlwriter_start_cdata);
ZEND_FUNCTION(xmlwriter_end_cdata);
ZEND_FUNCTION(xmlwriter_write_cdata);
ZEND_FUNCTION(xmlwriter_text);
ZEND_FUNCTION(xmlwriter_write_raw);
ZEND_FUNCTION(xmlwriter_start_document);
ZEND_FUNCTION(xmlwriter_end_document);
ZEND_FUNCTION(xmlwriter_write_comment);
ZEND_FUNCTION(xmlwriter_start_dtd);
ZEND_FUNCTION(xmlwriter_end_dtd);
ZEND_FUNCTION(xmlwriter_write_dtd);
ZEND_FUNCTION(xmlwriter_start_dtd_element);
ZEND_FUNCTION(xmlwriter_end_dtd_element);
ZEND_FUNCTION(xmlwriter_write_dtd_element);
ZEND_FUNCTION(xmlwriter_start_dtd_attlist);
ZEND_FUNCTION(xmlwriter_end_dtd_attlist);
ZEND_FUNCTION(xmlwriter_write_dtd_attlist);
ZEND_FUNCTION(xmlwriter_start_dtd_entity);
ZEND_FUNCTION(xmlwriter_end_dtd_entity);
ZEND_FUNCTION(xmlwriter_write_dtd_entity);
ZEND_FUNCTION(xmlwriter_output_memory);
ZEND_FUNCTION(xmlwriter_flush);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(xmlwriter_open_uri, arginfo_xmlwriter_open_uri)
	ZEND_FE(xmlwriter_open_memory, arginfo_xmlwriter_open_memory)
	ZEND_FE(xmlwriter_set_indent, arginfo_xmlwriter_set_indent)
	ZEND_FE(xmlwriter_set_indent_string, arginfo_xmlwriter_set_indent_string)
	ZEND_FE(xmlwriter_start_comment, arginfo_xmlwriter_start_comment)
	ZEND_FE(xmlwriter_end_comment, arginfo_xmlwriter_end_comment)
	ZEND_FE(xmlwriter_start_attribute, arginfo_xmlwriter_start_attribute)
	ZEND_FE(xmlwriter_end_attribute, arginfo_xmlwriter_end_attribute)
	ZEND_FE(xmlwriter_write_attribute, arginfo_xmlwriter_write_attribute)
	ZEND_FE(xmlwriter_start_attribute_ns, arginfo_xmlwriter_start_attribute_ns)
	ZEND_FE(xmlwriter_write_attribute_ns, arginfo_xmlwriter_write_attribute_ns)
	ZEND_FE(xmlwriter_start_element, arginfo_xmlwriter_start_element)
	ZEND_FE(xmlwriter_end_element, arginfo_xmlwriter_end_element)
	ZEND_FE(xmlwriter_full_end_element, arginfo_xmlwriter_full_end_element)
	ZEND_FE(xmlwriter_start_element_ns, arginfo_xmlwriter_start_element_ns)
	ZEND_FE(xmlwriter_write_element, arginfo_xmlwriter_write_element)
	ZEND_FE(xmlwriter_write_element_ns, arginfo_xmlwriter_write_element_ns)
	ZEND_FE(xmlwriter_start_pi, arginfo_xmlwriter_start_pi)
	ZEND_FE(xmlwriter_end_pi, arginfo_xmlwriter_end_pi)
	ZEND_FE(xmlwriter_write_pi, arginfo_xmlwriter_write_pi)
	ZEND_FE(xmlwriter_start_cdata, arginfo_xmlwriter_start_cdata)
	ZEND_FE(xmlwriter_end_cdata, arginfo_xmlwriter_end_cdata)
	ZEND_FE(xmlwriter_write_cdata, arginfo_xmlwriter_write_cdata)
	ZEND_FE(xmlwriter_text, arginfo_xmlwriter_text)
	ZEND_FE(xmlwriter_write_raw, arginfo_xmlwriter_write_raw)
	ZEND_FE(xmlwriter_start_document, arginfo_xmlwriter_start_document)
	ZEND_FE(xmlwriter_end_document, arginfo_xmlwriter_end_document)
	ZEND_FE(xmlwriter_write_comment, arginfo_xmlwriter_write_comment)
	ZEND_FE(xmlwriter_start_dtd, arginfo_xmlwriter_start_dtd)
	ZEND_FE(xmlwriter_end_dtd, arginfo_xmlwriter_end_dtd)
	ZEND_FE(xmlwriter_write_dtd, arginfo_xmlwriter_write_dtd)
	ZEND_FE(xmlwriter_start_dtd_element, arginfo_xmlwriter_start_dtd_element)
	ZEND_FE(xmlwriter_end_dtd_element, arginfo_xmlwriter_end_dtd_element)
	ZEND_FE(xmlwriter_write_dtd_element, arginfo_xmlwriter_write_dtd_element)
	ZEND_FE(xmlwriter_start_dtd_attlist, arginfo_xmlwriter_start_dtd_attlist)
	ZEND_FE(xmlwriter_end_dtd_attlist, arginfo_xmlwriter_end_dtd_attlist)
	ZEND_FE(xmlwriter_write_dtd_attlist, arginfo_xmlwriter_write_dtd_attlist)
	ZEND_FE(xmlwriter_start_dtd_entity, arginfo_xmlwriter_start_dtd_entity)
	ZEND_FE(xmlwriter_end_dtd_entity, arginfo_xmlwriter_end_dtd_entity)
	ZEND_FE(xmlwriter_write_dtd_entity, arginfo_xmlwriter_write_dtd_entity)
	ZEND_FE(xmlwriter_output_memory, arginfo_xmlwriter_output_memory)
	ZEND_FE(xmlwriter_flush, arginfo_xmlwriter_flush)
	ZEND_FE_END
};


static const zend_function_entry class_XMLWriter_methods[] = {
	ZEND_ME_MAPPING(openUri, xmlwriter_open_uri, arginfo_class_XMLWriter_openUri, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(openMemory, xmlwriter_open_memory, arginfo_class_XMLWriter_openMemory, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setIndent, xmlwriter_set_indent, arginfo_class_XMLWriter_setIndent, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setIndentString, xmlwriter_set_indent_string, arginfo_class_XMLWriter_setIndentString, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startComment, xmlwriter_start_comment, arginfo_class_XMLWriter_startComment, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endComment, xmlwriter_end_comment, arginfo_class_XMLWriter_endComment, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startAttribute, xmlwriter_start_attribute, arginfo_class_XMLWriter_startAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endAttribute, xmlwriter_end_attribute, arginfo_class_XMLWriter_endAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeAttribute, xmlwriter_write_attribute, arginfo_class_XMLWriter_writeAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startAttributeNs, xmlwriter_start_attribute_ns, arginfo_class_XMLWriter_startAttributeNs, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeAttributeNs, xmlwriter_write_attribute_ns, arginfo_class_XMLWriter_writeAttributeNs, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startElement, xmlwriter_start_element, arginfo_class_XMLWriter_startElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endElement, xmlwriter_end_element, arginfo_class_XMLWriter_endElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fullEndElement, xmlwriter_full_end_element, arginfo_class_XMLWriter_fullEndElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startElementNs, xmlwriter_start_element_ns, arginfo_class_XMLWriter_startElementNs, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeElement, xmlwriter_write_element, arginfo_class_XMLWriter_writeElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeElementNs, xmlwriter_write_element_ns, arginfo_class_XMLWriter_writeElementNs, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startPi, xmlwriter_start_pi, arginfo_class_XMLWriter_startPi, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endPi, xmlwriter_end_pi, arginfo_class_XMLWriter_endPi, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writePi, xmlwriter_write_pi, arginfo_class_XMLWriter_writePi, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startCdata, xmlwriter_start_cdata, arginfo_class_XMLWriter_startCdata, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endCdata, xmlwriter_end_cdata, arginfo_class_XMLWriter_endCdata, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeCdata, xmlwriter_write_cdata, arginfo_class_XMLWriter_writeCdata, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(text, xmlwriter_text, arginfo_class_XMLWriter_text, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeRaw, xmlwriter_write_raw, arginfo_class_XMLWriter_writeRaw, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startDocument, xmlwriter_start_document, arginfo_class_XMLWriter_startDocument, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endDocument, xmlwriter_end_document, arginfo_class_XMLWriter_endDocument, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeComment, xmlwriter_write_comment, arginfo_class_XMLWriter_writeComment, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startDtd, xmlwriter_start_dtd, arginfo_class_XMLWriter_startDtd, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endDtd, xmlwriter_end_dtd, arginfo_class_XMLWriter_endDtd, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeDtd, xmlwriter_write_dtd, arginfo_class_XMLWriter_writeDtd, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startDtdElement, xmlwriter_start_dtd_element, arginfo_class_XMLWriter_startDtdElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endDtdElement, xmlwriter_end_dtd_element, arginfo_class_XMLWriter_endDtdElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeDtdElement, xmlwriter_write_dtd_element, arginfo_class_XMLWriter_writeDtdElement, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startDtdAttlist, xmlwriter_start_dtd_attlist, arginfo_class_XMLWriter_startDtdAttlist, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endDtdAttlist, xmlwriter_end_dtd_attlist, arginfo_class_XMLWriter_endDtdAttlist, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeDtdAttlist, xmlwriter_write_dtd_attlist, arginfo_class_XMLWriter_writeDtdAttlist, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(startDtdEntity, xmlwriter_start_dtd_entity, arginfo_class_XMLWriter_startDtdEntity, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(endDtdEntity, xmlwriter_end_dtd_entity, arginfo_class_XMLWriter_endDtdEntity, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(writeDtdEntity, xmlwriter_write_dtd_entity, arginfo_class_XMLWriter_writeDtdEntity, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(outputMemory, xmlwriter_output_memory, arginfo_class_XMLWriter_outputMemory, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(flush, xmlwriter_flush, arginfo_class_XMLWriter_flush, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
