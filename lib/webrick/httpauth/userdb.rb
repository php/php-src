#--
# httpauth/userdb.rb -- UserDB mix-in module.
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: userdb.rb,v 1.2 2003/02/20 07:15:48 gotoyuzo Exp $

module WEBrick
  module HTTPAuth

    ##
    # User database mixin for HTTPAuth.  This mixin dispatches user record
    # access to the underlying auth_type for this database.

    module UserDB

      ##
      # The authentication type.
      #
      # WEBrick::HTTPAuth::BasicAuth or WEBrick::HTTPAuth::DigestAuth are
      # built-in.

      attr_accessor :auth_type

      ##
      # Creates an obscured password in +realm+ with +user+ and +password+
      # using the auth_type of this database.

      def make_passwd(realm, user, pass)
        @auth_type::make_passwd(realm, user, pass)
      end

      ##
      # Sets a password in +realm+ with +user+ and +password+ for the
      # auth_type of this database.

      def set_passwd(realm, user, pass)
        self[user] = pass
      end

      ##
      # Retrieves a password in +realm+ for +user+ for the auth_type of this
      # database.  +reload_db+ is a dummy value.

      def get_passwd(realm, user, reload_db=false)
        make_passwd(realm, user, self[user])
      end
    end
  end
end
