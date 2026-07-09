/* This is a generated file, edit html.stub.php instead.
 * Stub hash: 4a645196f8983f4d938e5e2eb3b25f34d1401ccd */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Html_raw, 0, 1, Html\\Htmlable, 0)
	ZEND_ARG_TYPE_INFO(0, html, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Html_escape, 0, 1, Html\\Htmlable, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Html_transpile, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Html_render_component, 0, 1, Html\\Htmlable, 0)
	ZEND_ARG_TYPE_INFO(0, component, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, props, IS_ARRAY, 0, "[]")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, slot, Html\\Htmlable, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Html_render_dynamic, 0, 1, Html\\Htmlable, 0)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, attributes, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, children, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Html_register_component_factory, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, factory, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Html_unregister_component_factory, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, factory, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Html_register_component_decorator, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, decorator, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Html_unregister_component_decorator, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, decorator, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, component, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Html_Htmlable_toHtml, 0, 0, Html\\Htmlable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Html_Htmlable___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Html_Element___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tag, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, attributes, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, children, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_Html_Element_toHtml arginfo_class_Html_Htmlable_toHtml

#define arginfo_class_Html_Element___toString arginfo_class_Html_Htmlable___toString

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Html_Element_toDom, 0, 0, Dom\\DocumentFragment, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, document, Dom\\Document, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Html_Fragment___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, children, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_Html_Fragment_toHtml arginfo_class_Html_Htmlable_toHtml

#define arginfo_class_Html_Fragment___toString arginfo_class_Html_Htmlable___toString

#define arginfo_class_Html_Fragment_toDom arginfo_class_Html_Element_toDom

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Html_Raw___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, html, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Html_Raw_toHtml arginfo_class_Html_Htmlable_toHtml

#define arginfo_class_Html_Raw___toString arginfo_class_Html_Htmlable___toString

#define arginfo_class_Html_Raw_toDom arginfo_class_Html_Element_toDom

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Html_LazyFragment___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, thunk, Closure, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Html_LazyFragment_toHtml arginfo_class_Html_Htmlable_toHtml

#define arginfo_class_Html_LazyFragment___toString arginfo_class_Html_Htmlable___toString

ZEND_FUNCTION(Html_raw);
ZEND_FUNCTION(Html_escape);
ZEND_FUNCTION(Html_transpile);
ZEND_FUNCTION(Html_render_component);
ZEND_FUNCTION(Html_render_dynamic);
ZEND_FUNCTION(Html_register_component_factory);
ZEND_FUNCTION(Html_unregister_component_factory);
ZEND_FUNCTION(Html_register_component_decorator);
ZEND_FUNCTION(Html_unregister_component_decorator);
ZEND_METHOD(Html_Element, __construct);
ZEND_METHOD(Html_Element, toHtml);
ZEND_METHOD(Html_Element, __toString);
ZEND_METHOD(Html_Element, toDom);
ZEND_METHOD(Html_Fragment, __construct);
ZEND_METHOD(Html_Fragment, toHtml);
ZEND_METHOD(Html_Fragment, __toString);
ZEND_METHOD(Html_Fragment, toDom);
ZEND_METHOD(Html_Raw, __construct);
ZEND_METHOD(Html_Raw, toHtml);
ZEND_METHOD(Html_Raw, __toString);
ZEND_METHOD(Html_Raw, toDom);
ZEND_METHOD(Html_LazyFragment, __construct);
ZEND_METHOD(Html_LazyFragment, toHtml);
ZEND_METHOD(Html_LazyFragment, __toString);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "raw"), zif_Html_raw, arginfo_Html_raw, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "escape"), zif_Html_escape, arginfo_Html_escape, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "transpile"), zif_Html_transpile, arginfo_Html_transpile, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "render_component"), zif_Html_render_component, arginfo_Html_render_component, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "render_dynamic"), zif_Html_render_dynamic, arginfo_Html_render_dynamic, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "register_component_factory"), zif_Html_register_component_factory, arginfo_Html_register_component_factory, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "unregister_component_factory"), zif_Html_unregister_component_factory, arginfo_Html_unregister_component_factory, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "register_component_decorator"), zif_Html_register_component_decorator, arginfo_Html_register_component_decorator, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Html", "unregister_component_decorator"), zif_Html_unregister_component_decorator, arginfo_Html_unregister_component_decorator, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Html_Htmlable_methods[] = {
	ZEND_RAW_FENTRY("toHtml", NULL, arginfo_class_Html_Htmlable_toHtml, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("__toString", NULL, arginfo_class_Html_Htmlable___toString, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Html_Element_methods[] = {
	ZEND_ME(Html_Element, __construct, arginfo_class_Html_Element___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Element, toHtml, arginfo_class_Html_Element_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Element, __toString, arginfo_class_Html_Element___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Element, toDom, arginfo_class_Html_Element_toDom, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Html_Fragment_methods[] = {
	ZEND_ME(Html_Fragment, __construct, arginfo_class_Html_Fragment___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Fragment, toHtml, arginfo_class_Html_Fragment_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Fragment, __toString, arginfo_class_Html_Fragment___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Fragment, toDom, arginfo_class_Html_Fragment_toDom, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Html_Raw_methods[] = {
	ZEND_ME(Html_Raw, __construct, arginfo_class_Html_Raw___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Raw, toHtml, arginfo_class_Html_Raw_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Raw, __toString, arginfo_class_Html_Raw___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_Raw, toDom, arginfo_class_Html_Raw_toDom, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Html_LazyFragment_methods[] = {
	ZEND_ME(Html_LazyFragment, __construct, arginfo_class_Html_LazyFragment___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_LazyFragment, toHtml, arginfo_class_Html_LazyFragment_toHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Html_LazyFragment, __toString, arginfo_class_Html_LazyFragment___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Html_Htmlable(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Html", "Htmlable", class_Html_Htmlable_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_Html_Element(zend_class_entry *class_entry_Html_Htmlable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Html", "Element", class_Html_Element_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Html_Htmlable);

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

static zend_class_entry *register_class_Html_Fragment(zend_class_entry *class_entry_Html_Htmlable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Html", "Fragment", class_Html_Fragment_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Html_Htmlable);

	zval property_children_default_value;
	ZVAL_UNDEF(&property_children_default_value);
	zend_string *property_children_name = zend_string_init("children", sizeof("children") - 1, true);
	zend_declare_typed_property(class_entry, property_children_name, &property_children_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release_ex(property_children_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Html_Raw(zend_class_entry *class_entry_Html_Htmlable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Html", "Raw", class_Html_Raw_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Html_Htmlable);

	zval property_html_default_value;
	ZVAL_UNDEF(&property_html_default_value);
	zend_string *property_html_name = zend_string_init("html", sizeof("html") - 1, true);
	zend_declare_typed_property(class_entry, property_html_name, &property_html_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_html_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Html_Slot(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Html", "Slot", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zend_string *attribute_name_Attribute_class_Html_Slot_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, true);
	zend_attribute *attribute_Attribute_class_Html_Slot_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_Html_Slot_0, 1);
	zend_string_release_ex(attribute_name_Attribute_class_Html_Slot_0, true);
	ZVAL_LONG(&attribute_Attribute_class_Html_Slot_0->args[0].value, ZEND_ATTRIBUTE_TARGET_PARAMETER);

	return class_entry;
}

static zend_class_entry *register_class_Html_LazyFragment(zend_class_entry *class_entry_Html_Htmlable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Html", "LazyFragment", class_Html_LazyFragment_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES);
	zend_class_implements(class_entry, 1, class_entry_Html_Htmlable);

	return class_entry;
}
