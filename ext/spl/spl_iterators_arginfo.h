/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a56f02ad7b9578713f0d37b2cf3d95853a4ea45e */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_EmptyIterator_current, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_EmptyIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_EmptyIterator_key arginfo_class_EmptyIterator_current

#define arginfo_class_EmptyIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_EmptyIterator_rewind arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CallbackFilterIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CallbackFilterIterator_accept arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveCallbackFilterIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveCallbackFilterIterator_hasChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveCallbackFilterIterator_getChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIterator_hasChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIterator_getChildren arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveIteratorIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RecursiveIteratorIterator::LEAVES_ONLY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIteratorIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_key arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_current arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_getDepth arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveIteratorIterator_getSubIterator, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, level, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIteratorIterator_getInnerIterator arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_beginIteration arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_endIteration arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_callHasChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_callGetChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_beginChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_endChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveIteratorIterator_nextElement arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveIteratorIterator_setMaxDepth, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxDepth, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIteratorIterator_getMaxDepth arginfo_class_EmptyIterator_current

#define arginfo_class_OuterIterator_getInnerIterator arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IteratorIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IteratorIterator_getInnerIterator arginfo_class_EmptyIterator_current

#define arginfo_class_IteratorIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_IteratorIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_IteratorIterator_key arginfo_class_EmptyIterator_current

#define arginfo_class_IteratorIterator_current arginfo_class_EmptyIterator_current

#define arginfo_class_IteratorIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_FilterIterator_accept arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilterIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FilterIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_FilterIterator_next arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveFilterIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveFilterIterator_hasChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveFilterIterator_getChildren arginfo_class_EmptyIterator_current

#define arginfo_class_ParentIterator___construct arginfo_class_RecursiveFilterIterator___construct

#define arginfo_class_ParentIterator_accept arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SeekableIterator_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_LimitIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_LimitIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_LimitIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_LimitIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_LimitIterator_seek arginfo_class_SeekableIterator_seek

#define arginfo_class_LimitIterator_getPosition arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CachingIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "CachingIterator::CALL_TOSTRING")
ZEND_END_ARG_INFO()

#define arginfo_class_CachingIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_CachingIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_CachingIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_CachingIterator_hasNext arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CachingIterator_getFlags arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CachingIterator_setFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CachingIterator_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CachingIterator_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CachingIterator_offsetUnset arginfo_class_CachingIterator_offsetGet

#define arginfo_class_CachingIterator_offsetExists arginfo_class_CachingIterator_offsetGet

#define arginfo_class_CachingIterator_getCache arginfo_class_EmptyIterator_current

#define arginfo_class_CachingIterator_count arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveCachingIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "RecursiveCachingIterator::CALL_TOSTRING")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveCachingIterator_hasChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveCachingIterator_getChildren arginfo_class_EmptyIterator_current

#define arginfo_class_NoRewindIterator___construct arginfo_class_FilterIterator___construct

#define arginfo_class_NoRewindIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_NoRewindIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_NoRewindIterator_key arginfo_class_EmptyIterator_current

#define arginfo_class_NoRewindIterator_current arginfo_class_EmptyIterator_current

#define arginfo_class_NoRewindIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator___construct arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator_append arginfo_class_FilterIterator___construct

#define arginfo_class_AppendIterator_rewind arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator_valid arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator_current arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator_next arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator_getIteratorIndex arginfo_class_EmptyIterator_current

#define arginfo_class_AppendIterator_getArrayIterator arginfo_class_EmptyIterator_current

#define arginfo_class_InfiniteIterator___construct arginfo_class_FilterIterator___construct

#define arginfo_class_InfiniteIterator_next arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RegexIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RegexIterator::MATCH")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pregFlags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_RegexIterator_accept arginfo_class_EmptyIterator_current

#define arginfo_class_RegexIterator_getMode arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RegexIterator_setMode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RegexIterator_getFlags arginfo_class_EmptyIterator_current

#define arginfo_class_RegexIterator_setFlags arginfo_class_CachingIterator_setFlags

#define arginfo_class_RegexIterator_getRegex arginfo_class_EmptyIterator_current

#define arginfo_class_RegexIterator_getPregFlags arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RegexIterator_setPregFlags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, pregFlags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveRegexIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RecursiveRegexIterator::MATCH")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pregFlags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveRegexIterator_accept arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveRegexIterator_hasChildren arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveRegexIterator_getChildren arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveTreeIterator___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "RecursiveTreeIterator::BYPASS_KEY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cachingIteratorFlags, IS_LONG, 0, "CachingIterator::CATCH_GET_CHILD")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RecursiveTreeIterator::SELF_FIRST")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveTreeIterator_key arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveTreeIterator_current arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveTreeIterator_getPrefix arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveTreeIterator_setPostfix, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, postfix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveTreeIterator_setPrefixPart, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, part, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveTreeIterator_getEntry arginfo_class_EmptyIterator_current

#define arginfo_class_RecursiveTreeIterator_getPostfix arginfo_class_EmptyIterator_current


ZEND_METHOD(EmptyIterator, current);
ZEND_METHOD(EmptyIterator, next);
ZEND_METHOD(EmptyIterator, key);
ZEND_METHOD(EmptyIterator, valid);
ZEND_METHOD(EmptyIterator, rewind);
ZEND_METHOD(CallbackFilterIterator, __construct);
ZEND_METHOD(CallbackFilterIterator, accept);
ZEND_METHOD(RecursiveCallbackFilterIterator, __construct);
ZEND_METHOD(RecursiveFilterIterator, hasChildren);
ZEND_METHOD(RecursiveCallbackFilterIterator, getChildren);
ZEND_METHOD(RecursiveIteratorIterator, __construct);
ZEND_METHOD(RecursiveIteratorIterator, rewind);
ZEND_METHOD(RecursiveIteratorIterator, valid);
ZEND_METHOD(RecursiveIteratorIterator, key);
ZEND_METHOD(RecursiveIteratorIterator, current);
ZEND_METHOD(RecursiveIteratorIterator, next);
ZEND_METHOD(RecursiveIteratorIterator, getDepth);
ZEND_METHOD(RecursiveIteratorIterator, getSubIterator);
ZEND_METHOD(RecursiveIteratorIterator, getInnerIterator);
ZEND_METHOD(RecursiveIteratorIterator, beginIteration);
ZEND_METHOD(RecursiveIteratorIterator, endIteration);
ZEND_METHOD(RecursiveIteratorIterator, callHasChildren);
ZEND_METHOD(RecursiveIteratorIterator, callGetChildren);
ZEND_METHOD(RecursiveIteratorIterator, beginChildren);
ZEND_METHOD(RecursiveIteratorIterator, endChildren);
ZEND_METHOD(RecursiveIteratorIterator, nextElement);
ZEND_METHOD(RecursiveIteratorIterator, setMaxDepth);
ZEND_METHOD(RecursiveIteratorIterator, getMaxDepth);
ZEND_METHOD(IteratorIterator, __construct);
ZEND_METHOD(IteratorIterator, getInnerIterator);
ZEND_METHOD(IteratorIterator, rewind);
ZEND_METHOD(IteratorIterator, valid);
ZEND_METHOD(IteratorIterator, key);
ZEND_METHOD(IteratorIterator, current);
ZEND_METHOD(IteratorIterator, next);
ZEND_METHOD(FilterIterator, __construct);
ZEND_METHOD(FilterIterator, rewind);
ZEND_METHOD(FilterIterator, next);
ZEND_METHOD(RecursiveFilterIterator, __construct);
ZEND_METHOD(RecursiveFilterIterator, getChildren);
ZEND_METHOD(ParentIterator, __construct);
ZEND_METHOD(LimitIterator, __construct);
ZEND_METHOD(LimitIterator, rewind);
ZEND_METHOD(LimitIterator, valid);
ZEND_METHOD(LimitIterator, next);
ZEND_METHOD(LimitIterator, seek);
ZEND_METHOD(LimitIterator, getPosition);
ZEND_METHOD(CachingIterator, __construct);
ZEND_METHOD(CachingIterator, rewind);
ZEND_METHOD(CachingIterator, valid);
ZEND_METHOD(CachingIterator, next);
ZEND_METHOD(CachingIterator, hasNext);
ZEND_METHOD(CachingIterator, __toString);
ZEND_METHOD(CachingIterator, getFlags);
ZEND_METHOD(CachingIterator, setFlags);
ZEND_METHOD(CachingIterator, offsetGet);
ZEND_METHOD(CachingIterator, offsetSet);
ZEND_METHOD(CachingIterator, offsetUnset);
ZEND_METHOD(CachingIterator, offsetExists);
ZEND_METHOD(CachingIterator, getCache);
ZEND_METHOD(CachingIterator, count);
ZEND_METHOD(RecursiveCachingIterator, __construct);
ZEND_METHOD(RecursiveCachingIterator, hasChildren);
ZEND_METHOD(RecursiveCachingIterator, getChildren);
ZEND_METHOD(NoRewindIterator, __construct);
ZEND_METHOD(NoRewindIterator, rewind);
ZEND_METHOD(NoRewindIterator, valid);
ZEND_METHOD(NoRewindIterator, key);
ZEND_METHOD(NoRewindIterator, current);
ZEND_METHOD(NoRewindIterator, next);
ZEND_METHOD(AppendIterator, __construct);
ZEND_METHOD(AppendIterator, append);
ZEND_METHOD(AppendIterator, rewind);
ZEND_METHOD(AppendIterator, valid);
ZEND_METHOD(AppendIterator, current);
ZEND_METHOD(AppendIterator, next);
ZEND_METHOD(AppendIterator, getIteratorIndex);
ZEND_METHOD(AppendIterator, getArrayIterator);
ZEND_METHOD(InfiniteIterator, __construct);
ZEND_METHOD(InfiniteIterator, next);
ZEND_METHOD(RegexIterator, __construct);
ZEND_METHOD(RegexIterator, accept);
ZEND_METHOD(RegexIterator, getMode);
ZEND_METHOD(RegexIterator, setMode);
ZEND_METHOD(RegexIterator, getFlags);
ZEND_METHOD(RegexIterator, setFlags);
ZEND_METHOD(RegexIterator, getRegex);
ZEND_METHOD(RegexIterator, getPregFlags);
ZEND_METHOD(RegexIterator, setPregFlags);
ZEND_METHOD(RecursiveRegexIterator, __construct);
ZEND_METHOD(RecursiveRegexIterator, accept);
ZEND_METHOD(RecursiveRegexIterator, getChildren);
ZEND_METHOD(RecursiveTreeIterator, __construct);
ZEND_METHOD(RecursiveTreeIterator, key);
ZEND_METHOD(RecursiveTreeIterator, current);
ZEND_METHOD(RecursiveTreeIterator, getPrefix);
ZEND_METHOD(RecursiveTreeIterator, setPostfix);
ZEND_METHOD(RecursiveTreeIterator, setPrefixPart);
ZEND_METHOD(RecursiveTreeIterator, getEntry);
ZEND_METHOD(RecursiveTreeIterator, getPostfix);


static const zend_function_entry class_EmptyIterator_methods[] = {
	ZEND_ME(EmptyIterator, current, arginfo_class_EmptyIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(EmptyIterator, next, arginfo_class_EmptyIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(EmptyIterator, key, arginfo_class_EmptyIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(EmptyIterator, valid, arginfo_class_EmptyIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(EmptyIterator, rewind, arginfo_class_EmptyIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_CallbackFilterIterator_methods[] = {
	ZEND_ME(CallbackFilterIterator, __construct, arginfo_class_CallbackFilterIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CallbackFilterIterator, accept, arginfo_class_CallbackFilterIterator_accept, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveCallbackFilterIterator_methods[] = {
	ZEND_ME(RecursiveCallbackFilterIterator, __construct, arginfo_class_RecursiveCallbackFilterIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(RecursiveFilterIterator, hasChildren, hasChildren, arginfo_class_RecursiveCallbackFilterIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveCallbackFilterIterator, getChildren, arginfo_class_RecursiveCallbackFilterIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveIterator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(RecursiveIterator, hasChildren, arginfo_class_RecursiveIterator_hasChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(RecursiveIterator, getChildren, arginfo_class_RecursiveIterator_getChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveIteratorIterator_methods[] = {
	ZEND_ME(RecursiveIteratorIterator, __construct, arginfo_class_RecursiveIteratorIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, rewind, arginfo_class_RecursiveIteratorIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, valid, arginfo_class_RecursiveIteratorIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, key, arginfo_class_RecursiveIteratorIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, current, arginfo_class_RecursiveIteratorIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, next, arginfo_class_RecursiveIteratorIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, getDepth, arginfo_class_RecursiveIteratorIterator_getDepth, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, getSubIterator, arginfo_class_RecursiveIteratorIterator_getSubIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, getInnerIterator, arginfo_class_RecursiveIteratorIterator_getInnerIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, beginIteration, arginfo_class_RecursiveIteratorIterator_beginIteration, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, endIteration, arginfo_class_RecursiveIteratorIterator_endIteration, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, callHasChildren, arginfo_class_RecursiveIteratorIterator_callHasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, callGetChildren, arginfo_class_RecursiveIteratorIterator_callGetChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, beginChildren, arginfo_class_RecursiveIteratorIterator_beginChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, endChildren, arginfo_class_RecursiveIteratorIterator_endChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, nextElement, arginfo_class_RecursiveIteratorIterator_nextElement, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, setMaxDepth, arginfo_class_RecursiveIteratorIterator_setMaxDepth, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveIteratorIterator, getMaxDepth, arginfo_class_RecursiveIteratorIterator_getMaxDepth, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_OuterIterator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(OuterIterator, getInnerIterator, arginfo_class_OuterIterator_getInnerIterator, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_IteratorIterator_methods[] = {
	ZEND_ME(IteratorIterator, __construct, arginfo_class_IteratorIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(IteratorIterator, getInnerIterator, arginfo_class_IteratorIterator_getInnerIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(IteratorIterator, rewind, arginfo_class_IteratorIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(IteratorIterator, valid, arginfo_class_IteratorIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(IteratorIterator, key, arginfo_class_IteratorIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(IteratorIterator, current, arginfo_class_IteratorIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(IteratorIterator, next, arginfo_class_IteratorIterator_next, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_FilterIterator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(FilterIterator, accept, arginfo_class_FilterIterator_accept, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ME(FilterIterator, __construct, arginfo_class_FilterIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(FilterIterator, rewind, arginfo_class_FilterIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(FilterIterator, next, arginfo_class_FilterIterator_next, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveFilterIterator_methods[] = {
	ZEND_ME(RecursiveFilterIterator, __construct, arginfo_class_RecursiveFilterIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveFilterIterator, hasChildren, arginfo_class_RecursiveFilterIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveFilterIterator, getChildren, arginfo_class_RecursiveFilterIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ParentIterator_methods[] = {
	ZEND_ME(ParentIterator, __construct, arginfo_class_ParentIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(RecursiveFilterIterator, accept, hasChildren, arginfo_class_ParentIterator_accept, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SeekableIterator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeekableIterator, seek, arginfo_class_SeekableIterator_seek, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_LimitIterator_methods[] = {
	ZEND_ME(LimitIterator, __construct, arginfo_class_LimitIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(LimitIterator, rewind, arginfo_class_LimitIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(LimitIterator, valid, arginfo_class_LimitIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(LimitIterator, next, arginfo_class_LimitIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(LimitIterator, seek, arginfo_class_LimitIterator_seek, ZEND_ACC_PUBLIC)
	ZEND_ME(LimitIterator, getPosition, arginfo_class_LimitIterator_getPosition, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_CachingIterator_methods[] = {
	ZEND_ME(CachingIterator, __construct, arginfo_class_CachingIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, rewind, arginfo_class_CachingIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, valid, arginfo_class_CachingIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, next, arginfo_class_CachingIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, hasNext, arginfo_class_CachingIterator_hasNext, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, __toString, arginfo_class_CachingIterator___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, getFlags, arginfo_class_CachingIterator_getFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, setFlags, arginfo_class_CachingIterator_setFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, offsetGet, arginfo_class_CachingIterator_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, offsetSet, arginfo_class_CachingIterator_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, offsetUnset, arginfo_class_CachingIterator_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, offsetExists, arginfo_class_CachingIterator_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, getCache, arginfo_class_CachingIterator_getCache, ZEND_ACC_PUBLIC)
	ZEND_ME(CachingIterator, count, arginfo_class_CachingIterator_count, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveCachingIterator_methods[] = {
	ZEND_ME(RecursiveCachingIterator, __construct, arginfo_class_RecursiveCachingIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveCachingIterator, hasChildren, arginfo_class_RecursiveCachingIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveCachingIterator, getChildren, arginfo_class_RecursiveCachingIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_NoRewindIterator_methods[] = {
	ZEND_ME(NoRewindIterator, __construct, arginfo_class_NoRewindIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(NoRewindIterator, rewind, arginfo_class_NoRewindIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(NoRewindIterator, valid, arginfo_class_NoRewindIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(NoRewindIterator, key, arginfo_class_NoRewindIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(NoRewindIterator, current, arginfo_class_NoRewindIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(NoRewindIterator, next, arginfo_class_NoRewindIterator_next, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_AppendIterator_methods[] = {
	ZEND_ME(AppendIterator, __construct, arginfo_class_AppendIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, append, arginfo_class_AppendIterator_append, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, rewind, arginfo_class_AppendIterator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, valid, arginfo_class_AppendIterator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, current, arginfo_class_AppendIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, next, arginfo_class_AppendIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, getIteratorIndex, arginfo_class_AppendIterator_getIteratorIndex, ZEND_ACC_PUBLIC)
	ZEND_ME(AppendIterator, getArrayIterator, arginfo_class_AppendIterator_getArrayIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_InfiniteIterator_methods[] = {
	ZEND_ME(InfiniteIterator, __construct, arginfo_class_InfiniteIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(InfiniteIterator, next, arginfo_class_InfiniteIterator_next, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RegexIterator_methods[] = {
	ZEND_ME(RegexIterator, __construct, arginfo_class_RegexIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, accept, arginfo_class_RegexIterator_accept, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, getMode, arginfo_class_RegexIterator_getMode, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, setMode, arginfo_class_RegexIterator_setMode, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, getFlags, arginfo_class_RegexIterator_getFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, setFlags, arginfo_class_RegexIterator_setFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, getRegex, arginfo_class_RegexIterator_getRegex, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, getPregFlags, arginfo_class_RegexIterator_getPregFlags, ZEND_ACC_PUBLIC)
	ZEND_ME(RegexIterator, setPregFlags, arginfo_class_RegexIterator_setPregFlags, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveRegexIterator_methods[] = {
	ZEND_ME(RecursiveRegexIterator, __construct, arginfo_class_RecursiveRegexIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveRegexIterator, accept, arginfo_class_RecursiveRegexIterator_accept, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(RecursiveFilterIterator, hasChildren, hasChildren, arginfo_class_RecursiveRegexIterator_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveRegexIterator, getChildren, arginfo_class_RecursiveRegexIterator_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_RecursiveTreeIterator_methods[] = {
	ZEND_ME(RecursiveTreeIterator, __construct, arginfo_class_RecursiveTreeIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, key, arginfo_class_RecursiveTreeIterator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, current, arginfo_class_RecursiveTreeIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, getPrefix, arginfo_class_RecursiveTreeIterator_getPrefix, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, setPostfix, arginfo_class_RecursiveTreeIterator_setPostfix, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, setPrefixPart, arginfo_class_RecursiveTreeIterator_setPrefixPart, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, getEntry, arginfo_class_RecursiveTreeIterator_getEntry, ZEND_ACC_PUBLIC)
	ZEND_ME(RecursiveTreeIterator, getPostfix, arginfo_class_RecursiveTreeIterator_getPostfix, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
