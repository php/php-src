/* This is a generated file, edit io_poll.stub.php instead.
 * Stub hash: a7450146c5b3b3f3486611c83a55cf0cc932b27a */

#ifndef ZEND_IO_POLL_DECL_a7450146c5b3b3f3486611c83a55cf0cc932b27a_H
#define ZEND_IO_POLL_DECL_a7450146c5b3b3f3486611c83a55cf0cc932b27a_H

typedef enum zend_enum_Io_Poll_Backend {
	ZEND_ENUM_Io_Poll_Backend_Auto = 1,
	ZEND_ENUM_Io_Poll_Backend_Poll = 2,
	ZEND_ENUM_Io_Poll_Backend_Epoll = 3,
	ZEND_ENUM_Io_Poll_Backend_Kqueue = 4,
	ZEND_ENUM_Io_Poll_Backend_EventPorts = 5,
	ZEND_ENUM_Io_Poll_Backend_WSAPoll = 6,
} zend_enum_Io_Poll_Backend;

typedef enum zend_enum_Io_Poll_Event {
	ZEND_ENUM_Io_Poll_Event_Read = 1,
	ZEND_ENUM_Io_Poll_Event_Write = 2,
	ZEND_ENUM_Io_Poll_Event_Error = 3,
	ZEND_ENUM_Io_Poll_Event_HangUp = 4,
	ZEND_ENUM_Io_Poll_Event_ReadHangUp = 5,
	ZEND_ENUM_Io_Poll_Event_OneShot = 6,
	ZEND_ENUM_Io_Poll_Event_EdgeTriggered = 7,
} zend_enum_Io_Poll_Event;

#endif /* ZEND_IO_POLL_DECL_a7450146c5b3b3f3486611c83a55cf0cc932b27a_H */
