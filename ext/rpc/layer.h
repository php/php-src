#ifndef LAYER_H
#define LAYER_H

#include "handler.h"

RPC_DECLARE_HANDLER(com);

rpc_handler_entry handler_entries[] = {
	RPC_HANDLER(com)
};

#endif /* LAYER_H */