/* This is a generated file, edit io_hooks.stub.php instead.
 * Stub hash: 50d263a9ea9ac42721b53d6348ea137e40f5776f */

#ifndef ZEND_IO_HOOKS_DECL_50d263a9ea9ac42721b53d6348ea137e40f5776f_H
#define ZEND_IO_HOOKS_DECL_50d263a9ea9ac42721b53d6348ea137e40f5776f_H

typedef enum zend_enum_Io_CompletionStatus {
	ZEND_ENUM_Io_CompletionStatus_Done = 1,
	ZEND_ENUM_Io_CompletionStatus_Ready = 2,
	ZEND_ENUM_Io_CompletionStatus_Timeout = 3,
	ZEND_ENUM_Io_CompletionStatus_Interrupted = 4,
	ZEND_ENUM_Io_CompletionStatus_Cancelled = 5,
	ZEND_ENUM_Io_CompletionStatus_Unsupported = 6,
} zend_enum_Io_CompletionStatus;

typedef enum zend_enum_Io_Poll_Trigger {
	ZEND_ENUM_Io_Poll_Trigger_Edge = 1,
	ZEND_ENUM_Io_Poll_Trigger_Level = 2,
} zend_enum_Io_Poll_Trigger;

typedef enum zend_enum_Io_Hooks_Capability {
	ZEND_ENUM_Io_Hooks_Capability_Files = 1,
	ZEND_ENUM_Io_Hooks_Capability_DirectData = 2,
	ZEND_ENUM_Io_Hooks_Capability_DirectAccept = 3,
	ZEND_ENUM_Io_Hooks_Capability_EdgeRegistrations = 4,
	ZEND_ENUM_Io_Hooks_Capability_LevelRegistrations = 5,
} zend_enum_Io_Hooks_Capability;

#endif /* ZEND_IO_HOOKS_DECL_50d263a9ea9ac42721b53d6348ea137e40f5776f_H */
