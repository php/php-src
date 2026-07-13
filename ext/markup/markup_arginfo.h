/* This is a generated file, edit markup.stub.php instead.
 * Stub hash: 1143e1ea27bd1038bb2349cf90246b4d27c0ff62 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Markup_raw, 0, 1, Markup\\Html, 0)
	ZEND_ARG_TYPE_INFO(0, html, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Markup_escape, 0, 1, Markup\\Html, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Markup_render_component, 0, 1, Markup\\Html, 0)
	ZEND_ARG_TYPE_INFO(0, component, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, props, IS_ARRAY, 0, "[]")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, slot, Markup\\Html, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Markup_render_dynamic, 0, 1, Markup\\Html, 0)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, attributes, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, children, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Markup_register_component_factory, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, factory, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Markup_unregister_component_factory, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, factory, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Markup_register_component_decorator, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, decorator, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Markup_unregister_component_decorator, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, decorator, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Markup_Html_toHtml, 0, 0, Markup\\Html, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Markup_Html___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Markup_Element___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, attributes, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, children, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_Markup_Element_toHtml arginfo_class_Markup_Html_toHtml

#define arginfo_class_Markup_Element___toString arginfo_class_Markup_Html___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Markup_Fragment___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, children, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_Markup_Fragment_toHtml arginfo_class_Markup_Html_toHtml

#define arginfo_class_Markup_Fragment___toString arginfo_class_Markup_Html___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Markup_Raw___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, html, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Markup_Raw_toHtml arginfo_class_Markup_Html_toHtml

#define arginfo_class_Markup_Raw___toString arginfo_class_Markup_Html___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Markup_LazyFragment___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, thunk, Closure, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Markup_LazyFragment_toHtml arginfo_class_Markup_Html_toHtml

#define arginfo_class_Markup_LazyFragment___toString arginfo_class_Markup_Html___toString

ZEND_FUNCTION(Markup_raw);
ZEND_FUNCTION(Markup_escape);
ZEND_FUNCTION(Markup_render_component);
ZEND_FUNCTION(Markup_render_dynamic);
ZEND_FUNCTION(Markup_register_component_factory);
ZEND_FUNCTION(Markup_unregister_component_factory);
ZEND_FUNCTION(Markup_register_component_decorator);
ZEND_FUNCTION(Markup_unregister_component_decorator);
ZEND_METHOD(Markup_Element, __construct);
ZEND_METHOD(Markup_Element, toHtml);
ZEND_METHOD(Markup_Element, __toString);
ZEND_METHOD(Markup_Fragment, __construct);
ZEND_METHOD(Markup_Fragment, toHtml);
ZEND_METHOD(Markup_Fragment, __toString);
ZEND_METHOD(Markup_Raw, __construct);
ZEND_METHOD(Markup_Raw, toHtml);
ZEND_METHOD(Markup_Raw, __toString);
ZEND_METHOD(Markup_LazyFragment, __construct);
ZEND_METHOD(Markup_LazyFragment, toHtml);
ZEND_METHOD(Markup_LazyFragment, __toString);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "raw"), zif_Markup_raw, arginfo_Markup_raw, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "escape"), zif_Markup_escape, arginfo_Markup_escape, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "render_component"), zif_Markup_render_component, arginfo_Markup_render_component, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "render_dynamic"), zif_Markup_render_dynamic, arginfo_Markup_render_dynamic, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "register_component_factory"), zif_Markup_register_component_factory, arginfo_Markup_register_component_factory, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "unregister_component_factory"), zif_Markup_unregister_component_factory, arginfo_Markup_unregister_component_factory, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "register_component_decorator"), zif_Markup_register_component_decorator, arginfo_Markup_register_component_decorator, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Markup", "unregister_component_decorator"), zif_Markup_unregister_component_decorator, arginfo_Markup_unregister_component_decorator, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Markup_Html_methods[] = {
	ZEND_RAW_FENTRY("toHtml", NULL, arginfo_class_Markup_Html_toHtml, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("__toString", NULL, arginfo_class_Markup_Html___toString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Markup_Element_methods[] = {
	ZEND_ME(Markup_Element, __construct, arginfo_class_Markup_Element___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_Element, toHtml, arginfo_class_Markup_Element_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_Element, __toString, arginfo_class_Markup_Element___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Markup_Fragment_methods[] = {
	ZEND_ME(Markup_Fragment, __construct, arginfo_class_Markup_Fragment___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_Fragment, toHtml, arginfo_class_Markup_Fragment_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_Fragment, __toString, arginfo_class_Markup_Fragment___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Markup_Raw_methods[] = {
	ZEND_ME(Markup_Raw, __construct, arginfo_class_Markup_Raw___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_Raw, toHtml, arginfo_class_Markup_Raw_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_Raw, __toString, arginfo_class_Markup_Raw___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Markup_LazyFragment_methods[] = {
	ZEND_ME(Markup_LazyFragment, __construct, arginfo_class_Markup_LazyFragment___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_LazyFragment, toHtml, arginfo_class_Markup_LazyFragment_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Markup_LazyFragment, __toString, arginfo_class_Markup_LazyFragment___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Markup_Html(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Markup", "Html", class_Markup_Html_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_Markup_Element(zend_class_entry *class_entry_Markup_Html)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Markup", "Element", class_Markup_Element_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Markup_Html);

	zval property_tag_default_value;
	ZVAL_UNDEF(&property_tag_default_value);
	zend_string *property_tag_name = zend_string_init("tag", sizeof("tag") - 1, true);
	zend_declare_typed_property(class_entry, property_tag_name, &property_tag_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_tag_name, true);

	zval property_attributes_default_value;
	ZVAL_UNDEF(&property_attributes_default_value);
	zend_string *property_attributes_name = zend_string_init("attributes", sizeof("attributes") - 1, true);
	zend_declare_typed_property(class_entry, property_attributes_name, &property_attributes_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release_ex(property_attributes_name, true);

	zval property_children_default_value;
	ZVAL_UNDEF(&property_children_default_value);
	zend_string *property_children_name = zend_string_init("children", sizeof("children") - 1, true);
	zend_declare_typed_property(class_entry, property_children_name, &property_children_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release_ex(property_children_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Markup_Fragment(zend_class_entry *class_entry_Markup_Html)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Markup", "Fragment", class_Markup_Fragment_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Markup_Html);

	zval property_children_default_value;
	ZVAL_UNDEF(&property_children_default_value);
	zend_string *property_children_name = zend_string_init("children", sizeof("children") - 1, true);
	zend_declare_typed_property(class_entry, property_children_name, &property_children_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release_ex(property_children_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Markup_Raw(zend_class_entry *class_entry_Markup_Html)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Markup", "Raw", class_Markup_Raw_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Markup_Html);

	zval property_html_default_value;
	ZVAL_UNDEF(&property_html_default_value);
	zend_string *property_html_name = zend_string_init("html", sizeof("html") - 1, true);
	zend_declare_typed_property(class_entry, property_html_name, &property_html_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_html_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Markup_Slot(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Markup", "Slot", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zend_string *attribute_name_Attribute_class_Markup_Slot_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, true);
	zend_attribute *attribute_Attribute_class_Markup_Slot_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_Markup_Slot_0, 1);
	zend_string_release_ex(attribute_name_Attribute_class_Markup_Slot_0, true);
	ZVAL_LONG(&attribute_Attribute_class_Markup_Slot_0->args[0].value, ZEND_ATTRIBUTE_TARGET_PARAMETER);

	return class_entry;
}

static zend_class_entry *register_class_Markup_LazyFragment(zend_class_entry *class_entry_Markup_Html)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Markup", "LazyFragment", class_Markup_LazyFragment_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Markup_Html);

	return class_entry;
}
