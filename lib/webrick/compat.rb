#
# compat.rb -- cross platform compatibility
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2002 GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: compat.rb,v 1.6 2002/10/01 17:16:32 gotoyuzo Exp $

##
# System call error module used by webrick for cross platform compatability.
#
# EPROTO:: protocol error
# ECONNRESET:: remote host reset the connection request
# ECONNABORTED:: Client sent TCP reset (RST) before server has accepted the
#                connection requested by client.
#
module Errno
  ##
  # Protocol error.

  class EPROTO       < SystemCallError; end

  ##
  # Remote host reset the connection request.

  class ECONNRESET   < SystemCallError; end

  ##
  # Client sent TCP reset (RST) before server has accepted the connection
  # requested by client.

  class ECONNABORTED < SystemCallError; end
end
