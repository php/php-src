/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 68008c87b5dd1895761f51bff8f63df4b517a54b */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_EmptyIterator_current, 0, 0, IS_NEVER, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_EmptyIterator_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_EmptyIterator_key arginfo_class_EmptyIterator_current

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_EmptyIterator_valid, 0, 0, IS_FALSE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_EmptyIterator_rewind arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CallbackFilterIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CallbackFilterIterator_accept, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveCallbackFilterIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveCallbackFilterIterator_hasChildren arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveCallbackFilterIterator_getChildren, 0, 0, RecursiveCallbackFilterIterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIterator_hasChildren arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveIterator_getChildren, 0, 0, RecursiveIterator, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveIteratorIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RecursiveIteratorIterator::LEAVES_ONLY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIteratorIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_RecursiveIteratorIterator_valid arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RecursiveIteratorIterator_key, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIteratorIterator_current arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_RecursiveIteratorIterator_next arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RecursiveIteratorIterator_getDepth, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveIteratorIterator_getSubIterator, 0, 0, RecursiveIterator, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, level, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveIteratorIterator_getInnerIterator, 0, 0, RecursiveIterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveIteratorIterator_beginIteration arginfo_class_EmptyIterator_next

#define arginfo_class_RecursiveIteratorIterator_endIteration arginfo_class_EmptyIterator_next

#define arginfo_class_RecursiveIteratorIterator_callHasChildren arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_RecursiveIteratorIterator_callGetChildren arginfo_class_RecursiveIterator_getChildren

#define arginfo_class_RecursiveIteratorIterator_beginChildren arginfo_class_EmptyIterator_next

#define arginfo_class_RecursiveIteratorIterator_endChildren arginfo_class_EmptyIterator_next

#define arginfo_class_RecursiveIteratorIterator_nextElement arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RecursiveIteratorIterator_setMaxDepth, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxDepth, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_RecursiveIteratorIterator_getMaxDepth, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_OuterIterator_getInnerIterator, 0, 0, Iterator, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IteratorIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Traversable, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IteratorIterator_getInnerIterator arginfo_class_OuterIterator_getInnerIterator

#define arginfo_class_IteratorIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_IteratorIterator_valid arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_IteratorIterator_key arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_IteratorIterator_current arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_IteratorIterator_next arginfo_class_EmptyIterator_next

#define arginfo_class_FilterIterator_accept arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FilterIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_FilterIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_FilterIterator_next arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveFilterIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveFilterIterator_hasChildren arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveFilterIterator_getChildren, 0, 0, RecursiveFilterIterator, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_ParentIterator___construct arginfo_class_RecursiveFilterIterator___construct

#define arginfo_class_ParentIterator_accept arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SeekableIterator_seek, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_LimitIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_LimitIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_LimitIterator_valid arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_LimitIterator_next arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_LimitIterator_seek, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_LimitIterator_getPosition arginfo_class_RecursiveIteratorIterator_getDepth

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CachingIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "CachingIterator::CALL_TOSTRING")
ZEND_END_ARG_INFO()

#define arginfo_class_CachingIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_CachingIterator_valid arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_CachingIterator_next arginfo_class_EmptyIterator_next

#define arginfo_class_CachingIterator_hasNext arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CachingIterator_getFlags arginfo_class_RecursiveIteratorIterator_getDepth

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator_setFlags, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CachingIterator_getCache, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CachingIterator_count arginfo_class_RecursiveIteratorIterator_getDepth

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveCachingIterator___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "RecursiveCachingIterator::CALL_TOSTRING")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveCachingIterator_hasChildren arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveCachingIterator_getChildren, 0, 0, RecursiveCachingIterator, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_NoRewindIterator___construct arginfo_class_FilterIterator___construct

#define arginfo_class_NoRewindIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_NoRewindIterator_valid arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_NoRewindIterator_key arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_NoRewindIterator_current arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_NoRewindIterator_next arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_AppendIterator___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_AppendIterator_append, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_AppendIterator_rewind arginfo_class_EmptyIterator_next

#define arginfo_class_AppendIterator_valid arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_AppendIterator_current arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_AppendIterator_next arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_AppendIterator_getIteratorIndex, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_AppendIterator_getArrayIterator, 0, 0, ArrayIterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_InfiniteIterator___construct arginfo_class_FilterIterator___construct

#define arginfo_class_InfiniteIterator_next arginfo_class_EmptyIterator_next

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RegexIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RegexIterator::MATCH")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pregFlags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_RegexIterator_accept arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_RegexIterator_getMode arginfo_class_RecursiveIteratorIterator_getDepth

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RegexIterator_setMode, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RegexIterator_getFlags arginfo_class_RecursiveIteratorIterator_getDepth

#define arginfo_class_RegexIterator_setFlags arginfo_class_CachingIterator_setFlags

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RegexIterator_getRegex, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RegexIterator_getPregFlags arginfo_class_RecursiveIteratorIterator_getDepth

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RegexIterator_setPregFlags, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, pregFlags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveRegexIterator___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, iterator, RecursiveIterator, 0)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RecursiveRegexIterator::MATCH")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pregFlags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveRegexIterator_accept arginfo_class_CallbackFilterIterator_accept

#define arginfo_class_RecursiveRegexIterator_hasChildren arginfo_class_CallbackFilterIterator_accept

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_RecursiveRegexIterator_getChildren, 0, 0, RecursiveRegexIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RecursiveTreeIterator___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, iterator)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "RecursiveTreeIterator::BYPASS_KEY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cachingIteratorFlags, IS_LONG, 0, "CachingIterator::CATCH_GET_CHILD")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "RecursiveTreeIterator::SELF_FIRST")
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveTreeIterator_key arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_RecursiveTreeIterator_current arginfo_class_RecursiveIteratorIterator_key

#define arginfo_class_RecursiveTreeIterator_getPrefix arginfo_class_RegexIterator_getRegex

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RecursiveTreeIterator_setPostfix, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, postfix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_RecursiveTreeIterator_setPrefixPart, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, part, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RecursiveTreeIterator_getEntry arginfo_class_RegexIterator_getRegex

#define arginfo_class_RecursiveTreeIterator_getPostfix arginfo_class_RegexIterator_getRegex

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
	ZEND_RAW_FENTRY("current", zim_EmptyIterator_current, arginfo_class_EmptyIterator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_EmptyIterator_next, arginfo_class_EmptyIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_EmptyIterator_key, arginfo_class_EmptyIterator_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_EmptyIterator_valid, arginfo_class_EmptyIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_EmptyIterator_rewind, arginfo_class_EmptyIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_CallbackFilterIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_CallbackFilterIterator___construct, arginfo_class_CallbackFilterIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("accept", zim_CallbackFilterIterator_accept, arginfo_class_CallbackFilterIterator_accept, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveCallbackFilterIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RecursiveCallbackFilterIterator___construct, arginfo_class_RecursiveCallbackFilterIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("hasChildren", zim_RecursiveFilterIterator_hasChildren, arginfo_class_RecursiveCallbackFilterIterator_hasChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getChildren", zim_RecursiveCallbackFilterIterator_getChildren, arginfo_class_RecursiveCallbackFilterIterator_getChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveIterator_methods[] = {
	ZEND_RAW_FENTRY("hasChildren", NULL, arginfo_class_RecursiveIterator_hasChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("getChildren", NULL, arginfo_class_RecursiveIterator_getChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveIteratorIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RecursiveIteratorIterator___construct, arginfo_class_RecursiveIteratorIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_RecursiveIteratorIterator_rewind, arginfo_class_RecursiveIteratorIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_RecursiveIteratorIterator_valid, arginfo_class_RecursiveIteratorIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_RecursiveIteratorIterator_key, arginfo_class_RecursiveIteratorIterator_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_RecursiveIteratorIterator_current, arginfo_class_RecursiveIteratorIterator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_RecursiveIteratorIterator_next, arginfo_class_RecursiveIteratorIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getDepth", zim_RecursiveIteratorIterator_getDepth, arginfo_class_RecursiveIteratorIterator_getDepth, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getSubIterator", zim_RecursiveIteratorIterator_getSubIterator, arginfo_class_RecursiveIteratorIterator_getSubIterator, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getInnerIterator", zim_RecursiveIteratorIterator_getInnerIterator, arginfo_class_RecursiveIteratorIterator_getInnerIterator, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("beginIteration", zim_RecursiveIteratorIterator_beginIteration, arginfo_class_RecursiveIteratorIterator_beginIteration, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endIteration", zim_RecursiveIteratorIterator_endIteration, arginfo_class_RecursiveIteratorIterator_endIteration, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("callHasChildren", zim_RecursiveIteratorIterator_callHasChildren, arginfo_class_RecursiveIteratorIterator_callHasChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("callGetChildren", zim_RecursiveIteratorIterator_callGetChildren, arginfo_class_RecursiveIteratorIterator_callGetChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("beginChildren", zim_RecursiveIteratorIterator_beginChildren, arginfo_class_RecursiveIteratorIterator_beginChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("endChildren", zim_RecursiveIteratorIterator_endChildren, arginfo_class_RecursiveIteratorIterator_endChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("nextElement", zim_RecursiveIteratorIterator_nextElement, arginfo_class_RecursiveIteratorIterator_nextElement, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setMaxDepth", zim_RecursiveIteratorIterator_setMaxDepth, arginfo_class_RecursiveIteratorIterator_setMaxDepth, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getMaxDepth", zim_RecursiveIteratorIterator_getMaxDepth, arginfo_class_RecursiveIteratorIterator_getMaxDepth, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_OuterIterator_methods[] = {
	ZEND_RAW_FENTRY("getInnerIterator", NULL, arginfo_class_OuterIterator_getInnerIterator, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_IteratorIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_IteratorIterator___construct, arginfo_class_IteratorIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getInnerIterator", zim_IteratorIterator_getInnerIterator, arginfo_class_IteratorIterator_getInnerIterator, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_IteratorIterator_rewind, arginfo_class_IteratorIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_IteratorIterator_valid, arginfo_class_IteratorIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_IteratorIterator_key, arginfo_class_IteratorIterator_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_IteratorIterator_current, arginfo_class_IteratorIterator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_IteratorIterator_next, arginfo_class_IteratorIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_FilterIterator_methods[] = {
	ZEND_RAW_FENTRY("accept", NULL, arginfo_class_FilterIterator_accept, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("__construct", zim_FilterIterator___construct, arginfo_class_FilterIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_FilterIterator_rewind, arginfo_class_FilterIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_FilterIterator_next, arginfo_class_FilterIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveFilterIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RecursiveFilterIterator___construct, arginfo_class_RecursiveFilterIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("hasChildren", zim_RecursiveFilterIterator_hasChildren, arginfo_class_RecursiveFilterIterator_hasChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getChildren", zim_RecursiveFilterIterator_getChildren, arginfo_class_RecursiveFilterIterator_getChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_ParentIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_ParentIterator___construct, arginfo_class_ParentIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("accept", zim_RecursiveFilterIterator_hasChildren, arginfo_class_ParentIterator_accept, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SeekableIterator_methods[] = {
	ZEND_RAW_FENTRY("seek", NULL, arginfo_class_SeekableIterator_seek, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_LimitIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_LimitIterator___construct, arginfo_class_LimitIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_LimitIterator_rewind, arginfo_class_LimitIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_LimitIterator_valid, arginfo_class_LimitIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_LimitIterator_next, arginfo_class_LimitIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("seek", zim_LimitIterator_seek, arginfo_class_LimitIterator_seek, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getPosition", zim_LimitIterator_getPosition, arginfo_class_LimitIterator_getPosition, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_CachingIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_CachingIterator___construct, arginfo_class_CachingIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_CachingIterator_rewind, arginfo_class_CachingIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_CachingIterator_valid, arginfo_class_CachingIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_CachingIterator_next, arginfo_class_CachingIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("hasNext", zim_CachingIterator_hasNext, arginfo_class_CachingIterator_hasNext, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__toString", zim_CachingIterator___toString, arginfo_class_CachingIterator___toString, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getFlags", zim_CachingIterator_getFlags, arginfo_class_CachingIterator_getFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setFlags", zim_CachingIterator_setFlags, arginfo_class_CachingIterator_setFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetGet", zim_CachingIterator_offsetGet, arginfo_class_CachingIterator_offsetGet, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetSet", zim_CachingIterator_offsetSet, arginfo_class_CachingIterator_offsetSet, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetUnset", zim_CachingIterator_offsetUnset, arginfo_class_CachingIterator_offsetUnset, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("offsetExists", zim_CachingIterator_offsetExists, arginfo_class_CachingIterator_offsetExists, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getCache", zim_CachingIterator_getCache, arginfo_class_CachingIterator_getCache, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("count", zim_CachingIterator_count, arginfo_class_CachingIterator_count, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveCachingIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RecursiveCachingIterator___construct, arginfo_class_RecursiveCachingIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("hasChildren", zim_RecursiveCachingIterator_hasChildren, arginfo_class_RecursiveCachingIterator_hasChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getChildren", zim_RecursiveCachingIterator_getChildren, arginfo_class_RecursiveCachingIterator_getChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_NoRewindIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_NoRewindIterator___construct, arginfo_class_NoRewindIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_NoRewindIterator_rewind, arginfo_class_NoRewindIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_NoRewindIterator_valid, arginfo_class_NoRewindIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_NoRewindIterator_key, arginfo_class_NoRewindIterator_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_NoRewindIterator_current, arginfo_class_NoRewindIterator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_NoRewindIterator_next, arginfo_class_NoRewindIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_AppendIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_AppendIterator___construct, arginfo_class_AppendIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("append", zim_AppendIterator_append, arginfo_class_AppendIterator_append, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rewind", zim_AppendIterator_rewind, arginfo_class_AppendIterator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_AppendIterator_valid, arginfo_class_AppendIterator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_AppendIterator_current, arginfo_class_AppendIterator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_AppendIterator_next, arginfo_class_AppendIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getIteratorIndex", zim_AppendIterator_getIteratorIndex, arginfo_class_AppendIterator_getIteratorIndex, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getArrayIterator", zim_AppendIterator_getArrayIterator, arginfo_class_AppendIterator_getArrayIterator, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_InfiniteIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_InfiniteIterator___construct, arginfo_class_InfiniteIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_InfiniteIterator_next, arginfo_class_InfiniteIterator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RegexIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RegexIterator___construct, arginfo_class_RegexIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("accept", zim_RegexIterator_accept, arginfo_class_RegexIterator_accept, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getMode", zim_RegexIterator_getMode, arginfo_class_RegexIterator_getMode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setMode", zim_RegexIterator_setMode, arginfo_class_RegexIterator_setMode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getFlags", zim_RegexIterator_getFlags, arginfo_class_RegexIterator_getFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setFlags", zim_RegexIterator_setFlags, arginfo_class_RegexIterator_setFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getRegex", zim_RegexIterator_getRegex, arginfo_class_RegexIterator_getRegex, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getPregFlags", zim_RegexIterator_getPregFlags, arginfo_class_RegexIterator_getPregFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setPregFlags", zim_RegexIterator_setPregFlags, arginfo_class_RegexIterator_setPregFlags, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveRegexIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RecursiveRegexIterator___construct, arginfo_class_RecursiveRegexIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("accept", zim_RecursiveRegexIterator_accept, arginfo_class_RecursiveRegexIterator_accept, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("hasChildren", zim_RecursiveFilterIterator_hasChildren, arginfo_class_RecursiveRegexIterator_hasChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getChildren", zim_RecursiveRegexIterator_getChildren, arginfo_class_RecursiveRegexIterator_getChildren, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_RecursiveTreeIterator_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_RecursiveTreeIterator___construct, arginfo_class_RecursiveTreeIterator___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_RecursiveTreeIterator_key, arginfo_class_RecursiveTreeIterator_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_RecursiveTreeIterator_current, arginfo_class_RecursiveTreeIterator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getPrefix", zim_RecursiveTreeIterator_getPrefix, arginfo_class_RecursiveTreeIterator_getPrefix, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setPostfix", zim_RecursiveTreeIterator_setPostfix, arginfo_class_RecursiveTreeIterator_setPostfix, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setPrefixPart", zim_RecursiveTreeIterator_setPrefixPart, arginfo_class_RecursiveTreeIterator_setPrefixPart, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getEntry", zim_RecursiveTreeIterator_getEntry, arginfo_class_RecursiveTreeIterator_getEntry, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getPostfix", zim_RecursiveTreeIterator_getPostfix, arginfo_class_RecursiveTreeIterator_getPostfix, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_EmptyIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "EmptyIterator", class_EmptyIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}

static zend_class_entry *register_class_CallbackFilterIterator(zend_class_entry *class_entry_FilterIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CallbackFilterIterator", class_CallbackFilterIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_FilterIterator);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveCallbackFilterIterator(zend_class_entry *class_entry_CallbackFilterIterator, zend_class_entry *class_entry_RecursiveIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveCallbackFilterIterator", class_RecursiveCallbackFilterIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_CallbackFilterIterator);
	zend_class_implements(class_entry, 1, class_entry_RecursiveIterator);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveIterator", class_RecursiveIterator_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveIteratorIterator(zend_class_entry *class_entry_OuterIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveIteratorIterator", class_RecursiveIteratorIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_OuterIterator);

	zval const_LEAVES_ONLY_value;
	ZVAL_LONG(&const_LEAVES_ONLY_value, RIT_LEAVES_ONLY);
	zend_string *const_LEAVES_ONLY_name = zend_string_init_interned("LEAVES_ONLY", sizeof("LEAVES_ONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LEAVES_ONLY_name, &const_LEAVES_ONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LEAVES_ONLY_name);

	zval const_SELF_FIRST_value;
	ZVAL_LONG(&const_SELF_FIRST_value, RIT_SELF_FIRST);
	zend_string *const_SELF_FIRST_name = zend_string_init_interned("SELF_FIRST", sizeof("SELF_FIRST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SELF_FIRST_name, &const_SELF_FIRST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SELF_FIRST_name);

	zval const_CHILD_FIRST_value;
	ZVAL_LONG(&const_CHILD_FIRST_value, RIT_CHILD_FIRST);
	zend_string *const_CHILD_FIRST_name = zend_string_init_interned("CHILD_FIRST", sizeof("CHILD_FIRST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHILD_FIRST_name, &const_CHILD_FIRST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHILD_FIRST_name);

	zval const_CATCH_GET_CHILD_value;
	ZVAL_LONG(&const_CATCH_GET_CHILD_value, RIT_CATCH_GET_CHILD);
	zend_string *const_CATCH_GET_CHILD_name = zend_string_init_interned("CATCH_GET_CHILD", sizeof("CATCH_GET_CHILD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CATCH_GET_CHILD_name, &const_CATCH_GET_CHILD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CATCH_GET_CHILD_name);

	return class_entry;
}

static zend_class_entry *register_class_OuterIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "OuterIterator", class_OuterIterator_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}

static zend_class_entry *register_class_IteratorIterator(zend_class_entry *class_entry_OuterIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IteratorIterator", class_IteratorIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_OuterIterator);

	return class_entry;
}

static zend_class_entry *register_class_FilterIterator(zend_class_entry *class_entry_IteratorIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "FilterIterator", class_FilterIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IteratorIterator);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;

	return class_entry;
}

static zend_class_entry *register_class_RecursiveFilterIterator(zend_class_entry *class_entry_FilterIterator, zend_class_entry *class_entry_RecursiveIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveFilterIterator", class_RecursiveFilterIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_FilterIterator);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	zend_class_implements(class_entry, 1, class_entry_RecursiveIterator);

	return class_entry;
}

static zend_class_entry *register_class_ParentIterator(zend_class_entry *class_entry_RecursiveFilterIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ParentIterator", class_ParentIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RecursiveFilterIterator);

	return class_entry;
}

static zend_class_entry *register_class_SeekableIterator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SeekableIterator", class_SeekableIterator_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}

static zend_class_entry *register_class_LimitIterator(zend_class_entry *class_entry_IteratorIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LimitIterator", class_LimitIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IteratorIterator);

	return class_entry;
}

static zend_class_entry *register_class_CachingIterator(zend_class_entry *class_entry_IteratorIterator, zend_class_entry *class_entry_ArrayAccess, zend_class_entry *class_entry_Countable, zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CachingIterator", class_CachingIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IteratorIterator);
	zend_class_implements(class_entry, 3, class_entry_ArrayAccess, class_entry_Countable, class_entry_Stringable);

	zval const_CALL_TOSTRING_value;
	ZVAL_LONG(&const_CALL_TOSTRING_value, CIT_CALL_TOSTRING);
	zend_string *const_CALL_TOSTRING_name = zend_string_init_interned("CALL_TOSTRING", sizeof("CALL_TOSTRING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CALL_TOSTRING_name, &const_CALL_TOSTRING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CALL_TOSTRING_name);

	zval const_CATCH_GET_CHILD_value;
	ZVAL_LONG(&const_CATCH_GET_CHILD_value, CIT_CATCH_GET_CHILD);
	zend_string *const_CATCH_GET_CHILD_name = zend_string_init_interned("CATCH_GET_CHILD", sizeof("CATCH_GET_CHILD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CATCH_GET_CHILD_name, &const_CATCH_GET_CHILD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CATCH_GET_CHILD_name);

	zval const_TOSTRING_USE_KEY_value;
	ZVAL_LONG(&const_TOSTRING_USE_KEY_value, CIT_TOSTRING_USE_KEY);
	zend_string *const_TOSTRING_USE_KEY_name = zend_string_init_interned("TOSTRING_USE_KEY", sizeof("TOSTRING_USE_KEY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TOSTRING_USE_KEY_name, &const_TOSTRING_USE_KEY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TOSTRING_USE_KEY_name);

	zval const_TOSTRING_USE_CURRENT_value;
	ZVAL_LONG(&const_TOSTRING_USE_CURRENT_value, CIT_TOSTRING_USE_CURRENT);
	zend_string *const_TOSTRING_USE_CURRENT_name = zend_string_init_interned("TOSTRING_USE_CURRENT", sizeof("TOSTRING_USE_CURRENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TOSTRING_USE_CURRENT_name, &const_TOSTRING_USE_CURRENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TOSTRING_USE_CURRENT_name);

	zval const_TOSTRING_USE_INNER_value;
	ZVAL_LONG(&const_TOSTRING_USE_INNER_value, CIT_TOSTRING_USE_INNER);
	zend_string *const_TOSTRING_USE_INNER_name = zend_string_init_interned("TOSTRING_USE_INNER", sizeof("TOSTRING_USE_INNER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TOSTRING_USE_INNER_name, &const_TOSTRING_USE_INNER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TOSTRING_USE_INNER_name);

	zval const_FULL_CACHE_value;
	ZVAL_LONG(&const_FULL_CACHE_value, CIT_FULL_CACHE);
	zend_string *const_FULL_CACHE_name = zend_string_init_interned("FULL_CACHE", sizeof("FULL_CACHE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FULL_CACHE_name, &const_FULL_CACHE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FULL_CACHE_name);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveCachingIterator(zend_class_entry *class_entry_CachingIterator, zend_class_entry *class_entry_RecursiveIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveCachingIterator", class_RecursiveCachingIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_CachingIterator);
	zend_class_implements(class_entry, 1, class_entry_RecursiveIterator);

	return class_entry;
}

static zend_class_entry *register_class_NoRewindIterator(zend_class_entry *class_entry_IteratorIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "NoRewindIterator", class_NoRewindIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IteratorIterator);

	return class_entry;
}

static zend_class_entry *register_class_AppendIterator(zend_class_entry *class_entry_IteratorIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "AppendIterator", class_AppendIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IteratorIterator);

	return class_entry;
}

static zend_class_entry *register_class_InfiniteIterator(zend_class_entry *class_entry_IteratorIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "InfiniteIterator", class_InfiniteIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_IteratorIterator);

	return class_entry;
}

static zend_class_entry *register_class_RegexIterator(zend_class_entry *class_entry_FilterIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RegexIterator", class_RegexIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_FilterIterator);

	zval const_USE_KEY_value;
	ZVAL_LONG(&const_USE_KEY_value, REGIT_USE_KEY);
	zend_string *const_USE_KEY_name = zend_string_init_interned("USE_KEY", sizeof("USE_KEY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_USE_KEY_name, &const_USE_KEY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_USE_KEY_name);

	zval const_INVERT_MATCH_value;
	ZVAL_LONG(&const_INVERT_MATCH_value, REGIT_INVERTED);
	zend_string *const_INVERT_MATCH_name = zend_string_init_interned("INVERT_MATCH", sizeof("INVERT_MATCH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_INVERT_MATCH_name, &const_INVERT_MATCH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_INVERT_MATCH_name);

	zval const_MATCH_value;
	ZVAL_LONG(&const_MATCH_value, REGIT_MODE_MATCH);
	zend_string *const_MATCH_name = zend_string_init_interned("MATCH", sizeof("MATCH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_MATCH_name, &const_MATCH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_MATCH_name);

	zval const_GET_MATCH_value;
	ZVAL_LONG(&const_GET_MATCH_value, REGIT_MODE_GET_MATCH);
	zend_string *const_GET_MATCH_name = zend_string_init_interned("GET_MATCH", sizeof("GET_MATCH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GET_MATCH_name, &const_GET_MATCH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GET_MATCH_name);

	zval const_ALL_MATCHES_value;
	ZVAL_LONG(&const_ALL_MATCHES_value, REGIT_MODE_ALL_MATCHES);
	zend_string *const_ALL_MATCHES_name = zend_string_init_interned("ALL_MATCHES", sizeof("ALL_MATCHES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ALL_MATCHES_name, &const_ALL_MATCHES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ALL_MATCHES_name);

	zval const_SPLIT_value;
	ZVAL_LONG(&const_SPLIT_value, REGIT_MODE_SPLIT);
	zend_string *const_SPLIT_name = zend_string_init_interned("SPLIT", sizeof("SPLIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SPLIT_name, &const_SPLIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SPLIT_name);

	zval const_REPLACE_value;
	ZVAL_LONG(&const_REPLACE_value, REGIT_MODE_REPLACE);
	zend_string *const_REPLACE_name = zend_string_init_interned("REPLACE", sizeof("REPLACE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_REPLACE_name, &const_REPLACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_REPLACE_name);

	zval property_replacement_default_value;
	ZVAL_NULL(&property_replacement_default_value);
	zend_string *property_replacement_name = zend_string_init("replacement", sizeof("replacement") - 1, 1);
	zend_declare_typed_property(class_entry, property_replacement_name, &property_replacement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_replacement_name);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveRegexIterator(zend_class_entry *class_entry_RegexIterator, zend_class_entry *class_entry_RecursiveIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveRegexIterator", class_RecursiveRegexIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RegexIterator);
	zend_class_implements(class_entry, 1, class_entry_RecursiveIterator);

	return class_entry;
}

static zend_class_entry *register_class_RecursiveTreeIterator(zend_class_entry *class_entry_RecursiveIteratorIterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "RecursiveTreeIterator", class_RecursiveTreeIterator_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RecursiveIteratorIterator);

	zval const_BYPASS_CURRENT_value;
	ZVAL_LONG(&const_BYPASS_CURRENT_value, RTIT_BYPASS_CURRENT);
	zend_string *const_BYPASS_CURRENT_name = zend_string_init_interned("BYPASS_CURRENT", sizeof("BYPASS_CURRENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BYPASS_CURRENT_name, &const_BYPASS_CURRENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BYPASS_CURRENT_name);

	zval const_BYPASS_KEY_value;
	ZVAL_LONG(&const_BYPASS_KEY_value, RTIT_BYPASS_KEY);
	zend_string *const_BYPASS_KEY_name = zend_string_init_interned("BYPASS_KEY", sizeof("BYPASS_KEY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BYPASS_KEY_name, &const_BYPASS_KEY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BYPASS_KEY_name);

	zval const_PREFIX_LEFT_value;
	ZVAL_LONG(&const_PREFIX_LEFT_value, 0);
	zend_string *const_PREFIX_LEFT_name = zend_string_init_interned("PREFIX_LEFT", sizeof("PREFIX_LEFT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PREFIX_LEFT_name, &const_PREFIX_LEFT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PREFIX_LEFT_name);

	zval const_PREFIX_MID_HAS_NEXT_value;
	ZVAL_LONG(&const_PREFIX_MID_HAS_NEXT_value, 1);
	zend_string *const_PREFIX_MID_HAS_NEXT_name = zend_string_init_interned("PREFIX_MID_HAS_NEXT", sizeof("PREFIX_MID_HAS_NEXT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PREFIX_MID_HAS_NEXT_name, &const_PREFIX_MID_HAS_NEXT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PREFIX_MID_HAS_NEXT_name);

	zval const_PREFIX_MID_LAST_value;
	ZVAL_LONG(&const_PREFIX_MID_LAST_value, 2);
	zend_string *const_PREFIX_MID_LAST_name = zend_string_init_interned("PREFIX_MID_LAST", sizeof("PREFIX_MID_LAST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PREFIX_MID_LAST_name, &const_PREFIX_MID_LAST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PREFIX_MID_LAST_name);

	zval const_PREFIX_END_HAS_NEXT_value;
	ZVAL_LONG(&const_PREFIX_END_HAS_NEXT_value, 3);
	zend_string *const_PREFIX_END_HAS_NEXT_name = zend_string_init_interned("PREFIX_END_HAS_NEXT", sizeof("PREFIX_END_HAS_NEXT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PREFIX_END_HAS_NEXT_name, &const_PREFIX_END_HAS_NEXT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PREFIX_END_HAS_NEXT_name);

	zval const_PREFIX_END_LAST_value;
	ZVAL_LONG(&const_PREFIX_END_LAST_value, 4);
	zend_string *const_PREFIX_END_LAST_name = zend_string_init_interned("PREFIX_END_LAST", sizeof("PREFIX_END_LAST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PREFIX_END_LAST_name, &const_PREFIX_END_LAST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PREFIX_END_LAST_name);

	zval const_PREFIX_RIGHT_value;
	ZVAL_LONG(&const_PREFIX_RIGHT_value, 5);
	zend_string *const_PREFIX_RIGHT_name = zend_string_init_interned("PREFIX_RIGHT", sizeof("PREFIX_RIGHT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PREFIX_RIGHT_name, &const_PREFIX_RIGHT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PREFIX_RIGHT_name);

	return class_entry;
}
