/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 820ad2d68166b189b9163c2c3dfcc76806d41b7d */

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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_openUri, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_openMemory, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_setIndent, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_setIndentString, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, indentation, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startComment arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_endComment arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startAttribute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endAttribute arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeAttribute, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startAttributeNs, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeAttributeNs, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startElement arginfo_class_XMLWriter_startAttribute

#define arginfo_class_XMLWriter_endElement arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_fullEndElement arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_startElementNs arginfo_class_XMLWriter_startAttributeNs

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeElement, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeElementNs, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startPi, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endPi arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writePi, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startCdata arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_endCdata arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeCdata, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_text arginfo_class_XMLWriter_writeCdata

#define arginfo_class_XMLWriter_writeRaw arginfo_class_XMLWriter_writeCdata

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDocument, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, version, IS_STRING, 1, "\"1.0\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, standalone, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDocument arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_writeComment arginfo_class_XMLWriter_writeCdata

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDtd, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtd arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeDtd, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, content, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDtdElement, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtdElement arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeDtdElement, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_startDtdAttlist arginfo_class_XMLWriter_startAttribute

#define arginfo_class_XMLWriter_endDtdAttlist arginfo_class_XMLWriter_openMemory

#define arginfo_class_XMLWriter_writeDtdAttlist arginfo_class_XMLWriter_writeDtdElement

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_startDtdEntity, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isParam, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLWriter_endDtdEntity arginfo_class_XMLWriter_openMemory

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_writeDtdEntity, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, isParam, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, notationData, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_XMLWriter_outputMemory, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flush, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_XMLWriter_flush, 0, 0, MAY_BE_STRING|MAY_BE_LONG)
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
	ZEND_RAW_FENTRY("xmlwriter_open_uri", zif_xmlwriter_open_uri, arginfo_xmlwriter_open_uri, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_open_memory", zif_xmlwriter_open_memory, arginfo_xmlwriter_open_memory, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_set_indent", zif_xmlwriter_set_indent, arginfo_xmlwriter_set_indent, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_set_indent_string", zif_xmlwriter_set_indent_string, arginfo_xmlwriter_set_indent_string, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_comment", zif_xmlwriter_start_comment, arginfo_xmlwriter_start_comment, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_comment", zif_xmlwriter_end_comment, arginfo_xmlwriter_end_comment, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_attribute", zif_xmlwriter_start_attribute, arginfo_xmlwriter_start_attribute, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_attribute", zif_xmlwriter_end_attribute, arginfo_xmlwriter_end_attribute, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_attribute", zif_xmlwriter_write_attribute, arginfo_xmlwriter_write_attribute, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_attribute_ns", zif_xmlwriter_start_attribute_ns, arginfo_xmlwriter_start_attribute_ns, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_attribute_ns", zif_xmlwriter_write_attribute_ns, arginfo_xmlwriter_write_attribute_ns, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_element", zif_xmlwriter_start_element, arginfo_xmlwriter_start_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_element", zif_xmlwriter_end_element, arginfo_xmlwriter_end_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_full_end_element", zif_xmlwriter_full_end_element, arginfo_xmlwriter_full_end_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_element_ns", zif_xmlwriter_start_element_ns, arginfo_xmlwriter_start_element_ns, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_element", zif_xmlwriter_write_element, arginfo_xmlwriter_write_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_element_ns", zif_xmlwriter_write_element_ns, arginfo_xmlwriter_write_element_ns, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_pi", zif_xmlwriter_start_pi, arginfo_xmlwriter_start_pi, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_pi", zif_xmlwriter_end_pi, arginfo_xmlwriter_end_pi, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_pi", zif_xmlwriter_write_pi, arginfo_xmlwriter_write_pi, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_cdata", zif_xmlwriter_start_cdata, arginfo_xmlwriter_start_cdata, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_cdata", zif_xmlwriter_end_cdata, arginfo_xmlwriter_end_cdata, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_cdata", zif_xmlwriter_write_cdata, arginfo_xmlwriter_write_cdata, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_text", zif_xmlwriter_text, arginfo_xmlwriter_text, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_raw", zif_xmlwriter_write_raw, arginfo_xmlwriter_write_raw, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_document", zif_xmlwriter_start_document, arginfo_xmlwriter_start_document, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_document", zif_xmlwriter_end_document, arginfo_xmlwriter_end_document, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_comment", zif_xmlwriter_write_comment, arginfo_xmlwriter_write_comment, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_dtd", zif_xmlwriter_start_dtd, arginfo_xmlwriter_start_dtd, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_dtd", zif_xmlwriter_end_dtd, arginfo_xmlwriter_end_dtd, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_dtd", zif_xmlwriter_write_dtd, arginfo_xmlwriter_write_dtd, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_dtd_element", zif_xmlwriter_start_dtd_element, arginfo_xmlwriter_start_dtd_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_dtd_element", zif_xmlwriter_end_dtd_element, arginfo_xmlwriter_end_dtd_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_dtd_element", zif_xmlwriter_write_dtd_element, arginfo_xmlwriter_write_dtd_element, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_dtd_attlist", zif_xmlwriter_start_dtd_attlist, arginfo_xmlwriter_start_dtd_attlist, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_dtd_attlist", zif_xmlwriter_end_dtd_attlist, arginfo_xmlwriter_end_dtd_attlist, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_dtd_attlist", zif_xmlwriter_write_dtd_attlist, arginfo_xmlwriter_write_dtd_attlist, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_start_dtd_entity", zif_xmlwriter_start_dtd_entity, arginfo_xmlwriter_start_dtd_entity, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_end_dtd_entity", zif_xmlwriter_end_dtd_entity, arginfo_xmlwriter_end_dtd_entity, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_write_dtd_entity", zif_xmlwriter_write_dtd_entity, arginfo_xmlwriter_write_dtd_entity, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_output_memory", zif_xmlwriter_output_memory, arginfo_xmlwriter_output_memory, 0, NULL)
	ZEND_RAW_FENTRY("xmlwriter_flush", zif_xmlwriter_flush, arginfo_xmlwriter_flush, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_XMLWriter_methods[] = {
	ZEND_RAW_FENTRY("openUri", zif_xmlwriter_open_uri, arginfo_class_XMLWriter_openUri, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("openMemory", zif_xmlwriter_open_memory, arginfo_class_XMLWriter_openMemory, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setIndent", zif_xmlwriter_set_indent, arginfo_class_XMLWriter_setIndent, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setIndentString", zif_xmlwriter_set_indent_string, arginfo_class_XMLWriter_setIndentString, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startComment", zif_xmlwriter_start_comment, arginfo_class_XMLWriter_startComment, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endComment", zif_xmlwriter_end_comment, arginfo_class_XMLWriter_endComment, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startAttribute", zif_xmlwriter_start_attribute, arginfo_class_XMLWriter_startAttribute, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endAttribute", zif_xmlwriter_end_attribute, arginfo_class_XMLWriter_endAttribute, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeAttribute", zif_xmlwriter_write_attribute, arginfo_class_XMLWriter_writeAttribute, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startAttributeNs", zif_xmlwriter_start_attribute_ns, arginfo_class_XMLWriter_startAttributeNs, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeAttributeNs", zif_xmlwriter_write_attribute_ns, arginfo_class_XMLWriter_writeAttributeNs, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startElement", zif_xmlwriter_start_element, arginfo_class_XMLWriter_startElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endElement", zif_xmlwriter_end_element, arginfo_class_XMLWriter_endElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("fullEndElement", zif_xmlwriter_full_end_element, arginfo_class_XMLWriter_fullEndElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startElementNs", zif_xmlwriter_start_element_ns, arginfo_class_XMLWriter_startElementNs, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeElement", zif_xmlwriter_write_element, arginfo_class_XMLWriter_writeElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeElementNs", zif_xmlwriter_write_element_ns, arginfo_class_XMLWriter_writeElementNs, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startPi", zif_xmlwriter_start_pi, arginfo_class_XMLWriter_startPi, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endPi", zif_xmlwriter_end_pi, arginfo_class_XMLWriter_endPi, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writePi", zif_xmlwriter_write_pi, arginfo_class_XMLWriter_writePi, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startCdata", zif_xmlwriter_start_cdata, arginfo_class_XMLWriter_startCdata, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endCdata", zif_xmlwriter_end_cdata, arginfo_class_XMLWriter_endCdata, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeCdata", zif_xmlwriter_write_cdata, arginfo_class_XMLWriter_writeCdata, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("text", zif_xmlwriter_text, arginfo_class_XMLWriter_text, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeRaw", zif_xmlwriter_write_raw, arginfo_class_XMLWriter_writeRaw, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startDocument", zif_xmlwriter_start_document, arginfo_class_XMLWriter_startDocument, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endDocument", zif_xmlwriter_end_document, arginfo_class_XMLWriter_endDocument, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeComment", zif_xmlwriter_write_comment, arginfo_class_XMLWriter_writeComment, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startDtd", zif_xmlwriter_start_dtd, arginfo_class_XMLWriter_startDtd, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endDtd", zif_xmlwriter_end_dtd, arginfo_class_XMLWriter_endDtd, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeDtd", zif_xmlwriter_write_dtd, arginfo_class_XMLWriter_writeDtd, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startDtdElement", zif_xmlwriter_start_dtd_element, arginfo_class_XMLWriter_startDtdElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endDtdElement", zif_xmlwriter_end_dtd_element, arginfo_class_XMLWriter_endDtdElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeDtdElement", zif_xmlwriter_write_dtd_element, arginfo_class_XMLWriter_writeDtdElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startDtdAttlist", zif_xmlwriter_start_dtd_attlist, arginfo_class_XMLWriter_startDtdAttlist, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endDtdAttlist", zif_xmlwriter_end_dtd_attlist, arginfo_class_XMLWriter_endDtdAttlist, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeDtdAttlist", zif_xmlwriter_write_dtd_attlist, arginfo_class_XMLWriter_writeDtdAttlist, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("startDtdEntity", zif_xmlwriter_start_dtd_entity, arginfo_class_XMLWriter_startDtdEntity, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endDtdEntity", zif_xmlwriter_end_dtd_entity, arginfo_class_XMLWriter_endDtdEntity, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("writeDtdEntity", zif_xmlwriter_write_dtd_entity, arginfo_class_XMLWriter_writeDtdEntity, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("outputMemory", zif_xmlwriter_output_memory, arginfo_class_XMLWriter_outputMemory, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("flush", zif_xmlwriter_flush, arginfo_class_XMLWriter_flush, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_XMLWriter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "XMLWriter", class_XMLWriter_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
