#
# = net/imap.rb
#
# Copyright (C) 2000  Shugo Maeda <shugo@ruby-lang.org>
#
# This library is distributed under the terms of the Ruby license.
# You can freely distribute/modify this library.
#
# Documentation: Shugo Maeda, with RDoc conversion and overview by William
# Webber.
#
# See Net::IMAP for documentation.
#


require "socket"
require "monitor"
require "digest/md5"
require "strscan"
begin
  require "openssl"
rescue LoadError
end

module Net

  #
  # Net::IMAP implements Internet Message Access Protocol (IMAP) client
  # functionality.  The protocol is described in [IMAP].
  #
  # == IMAP Overview
  #
  # An IMAP client connects to a server, and then authenticates
  # itself using either #authenticate() or #login().  Having
  # authenticated itself, there is a range of commands
  # available to it.  Most work with mailboxes, which may be
  # arranged in an hierarchical namespace, and each of which
  # contains zero or more messages.  How this is implemented on
  # the server is implementation-dependent; on a UNIX server, it
  # will frequently be implemented as a files in mailbox format
  # within a hierarchy of directories.
  #
  # To work on the messages within a mailbox, the client must
  # first select that mailbox, using either #select() or (for
  # read-only access) #examine().  Once the client has successfully
  # selected a mailbox, they enter _selected_ state, and that
  # mailbox becomes the _current_ mailbox, on which mail-item
  # related commands implicitly operate.
  #
  # Messages have two sorts of identifiers: message sequence
  # numbers, and UIDs.
  #
  # Message sequence numbers number messages within a mail box
  # from 1 up to the number of items in the mail box.  If new
  # message arrives during a session, it receives a sequence
  # number equal to the new size of the mail box.  If messages
  # are expunged from the mailbox, remaining messages have their
  # sequence numbers "shuffled down" to fill the gaps.
  #
  # UIDs, on the other hand, are permanently guaranteed not to
  # identify another message within the same mailbox, even if
  # the existing message is deleted.  UIDs are required to
  # be assigned in ascending (but not necessarily sequential)
  # order within a mailbox; this means that if a non-IMAP client
  # rearranges the order of mailitems within a mailbox, the
  # UIDs have to be reassigned.  An IMAP client cannot thus
  # rearrange message orders.
  #
  # == Examples of Usage
  #
  # === List sender and subject of all recent messages in the default mailbox
  #
  #   imap = Net::IMAP.new('mail.example.com')
  #   imap.authenticate('LOGIN', 'joe_user', 'joes_password')
  #   imap.examine('INBOX')
  #   imap.search(["RECENT"]).each do |message_id|
  #     envelope = imap.fetch(message_id, "ENVELOPE")[0].attr["ENVELOPE"]
  #     puts "#{envelope.from[0].name}: \t#{envelope.subject}"
  #   end
  #
  # === Move all messages from April 2003 from "Mail/sent-mail" to "Mail/sent-apr03"
  #
  #   imap = Net::IMAP.new('mail.example.com')
  #   imap.authenticate('LOGIN', 'joe_user', 'joes_password')
  #   imap.select('Mail/sent-mail')
  #   if not imap.list('Mail/', 'sent-apr03')
  #     imap.create('Mail/sent-apr03')
  #   end
  #   imap.search(["BEFORE", "30-Apr-2003", "SINCE", "1-Apr-2003"]).each do |message_id|
  #     imap.copy(message_id, "Mail/sent-apr03")
  #     imap.store(message_id, "+FLAGS", [:Deleted])
  #   end
  #   imap.expunge
  #
  # == Thread Safety
  #
  # Net::IMAP supports concurrent threads. For example,
  #
  #   imap = Net::IMAP.new("imap.foo.net", "imap2")
  #   imap.authenticate("cram-md5", "bar", "password")
  #   imap.select("inbox")
  #   fetch_thread = Thread.start { imap.fetch(1..-1, "UID") }
  #   search_result = imap.search(["BODY", "hello"])
  #   fetch_result = fetch_thread.value
  #   imap.disconnect
  #
  # This script invokes the FETCH command and the SEARCH command concurrently.
  #
  # == Errors
  #
  # An IMAP server can send three different types of responses to indicate
  # failure:
  #
  # NO:: the attempted command could not be successfully completed.  For
  #      instance, the username/password used for logging in are incorrect;
  #      the selected mailbox does not exists; etc.
  #
  # BAD:: the request from the client does not follow the server's
  #       understanding of the IMAP protocol.  This includes attempting
  #       commands from the wrong client state; for instance, attempting
  #       to perform a SEARCH command without having SELECTed a current
  #       mailbox.  It can also signal an internal server
  #       failure (such as a disk crash) has occurred.
  #
  # BYE:: the server is saying goodbye.  This can be part of a normal
  #       logout sequence, and can be used as part of a login sequence
  #       to indicate that the server is (for some reason) unwilling
  #       to accept our connection.  As a response to any other command,
  #       it indicates either that the server is shutting down, or that
  #       the server is timing out the client connection due to inactivity.
  #
  # These three error response are represented by the errors
  # Net::IMAP::NoResponseError, Net::IMAP::BadResponseError, and
  # Net::IMAP::ByeResponseError, all of which are subclasses of
  # Net::IMAP::ResponseError.  Essentially, all methods that involve
  # sending a request to the server can generate one of these errors.
  # Only the most pertinent instances have been documented below.
  #
  # Because the IMAP class uses Sockets for communication, its methods
  # are also susceptible to the various errors that can occur when
  # working with sockets.  These are generally represented as
  # Errno errors.  For instance, any method that involves sending a
  # request to the server and/or receiving a response from it could
  # raise an Errno::EPIPE error if the network connection unexpectedly
  # goes down.  See the socket(7), ip(7), tcp(7), socket(2), connect(2),
  # and associated man pages.
  #
  # Finally, a Net::IMAP::DataFormatError is thrown if low-level data
  # is found to be in an incorrect format (for instance, when converting
  # between UTF-8 and UTF-16), and Net::IMAP::ResponseParseError is
  # thrown if a server response is non-parseable.
  #
  #
  # == References
  #
  # [[IMAP]]
  #    M. Crispin, "INTERNET MESSAGE ACCESS PROTOCOL - VERSION 4rev1",
  #    RFC 2060, December 1996.  (Note: since obsoleted by RFC 3501)
  #
  # [[LANGUAGE-TAGS]]
  #    Alvestrand, H., "Tags for the Identification of
  #    Languages", RFC 1766, March 1995.
  #
  # [[MD5]]
  #    Myers, J., and M. Rose, "The Content-MD5 Header Field", RFC
  #    1864, October 1995.
  #
  # [[MIME-IMB]]
  #    Freed, N., and N. Borenstein, "MIME (Multipurpose Internet
  #    Mail Extensions) Part One: Format of Internet Message Bodies", RFC
  #    2045, November 1996.
  #
  # [[RFC-822]]
  #    Crocker, D., "Standard for the Format of ARPA Internet Text
  #    Messages", STD 11, RFC 822, University of Delaware, August 1982.
  #
  # [[RFC-2087]]
  #    Myers, J., "IMAP4 QUOTA extension", RFC 2087, January 1997.
  #
  # [[RFC-2086]]
  #    Myers, J., "IMAP4 ACL extension", RFC 2086, January 1997.
  #
  # [[RFC-2195]]
  #    Klensin, J., Catoe, R., and Krumviede, P., "IMAP/POP AUTHorize Extension
  #    for Simple Challenge/Response", RFC 2195, September 1997.
  #
  # [[SORT-THREAD-EXT]]
  #    Crispin, M., "INTERNET MESSAGE ACCESS PROTOCOL - SORT and THREAD
  #    Extensions", draft-ietf-imapext-sort, May 2003.
  #
  # [[OSSL]]
  #    http://www.openssl.org
  #
  # [[RSSL]]
  #    http://savannah.gnu.org/projects/rubypki
  #
  # [[UTF7]]
  #    Goldsmith, D. and Davis, M., "UTF-7: A Mail-Safe Transformation Format of
  #    Unicode", RFC 2152, May 1997.
  #
  class IMAP
    include MonitorMixin
    if defined?(OpenSSL)
      include OpenSSL
      include SSL
    end

    #  Returns an initial greeting response from the server.
    attr_reader :greeting

    # Returns recorded untagged responses.  For example:
    #
    #   imap.select("inbox")
    #   p imap.responses["EXISTS"][-1]
    #   #=> 2
    #   p imap.responses["UIDVALIDITY"][-1]
    #   #=> 968263756
    attr_reader :responses

    # Returns all response handlers.
    attr_reader :response_handlers

    # The thread to receive exceptions.
    attr_accessor :client_thread

    # Flag indicating a message has been seen
    SEEN = :Seen

    # Flag indicating a message has been answered
    ANSWERED = :Answered

    # Flag indicating a message has been flagged for special or urgent
    # attention
    FLAGGED = :Flagged

    # Flag indicating a message has been marked for deletion.  This
    # will occur when the mailbox is closed or expunged.
    DELETED = :Deleted

    # Flag indicating a message is only a draft or work-in-progress version.
    DRAFT = :Draft

    # Flag indicating that the message is "recent", meaning that this
    # session is the first session in which the client has been notified
    # of this message.
    RECENT = :Recent

    # Flag indicating that a mailbox context name cannot contain
    # children.
    NOINFERIORS = :Noinferiors

    # Flag indicating that a mailbox is not selected.
    NOSELECT = :Noselect

    # Flag indicating that a mailbox has been marked "interesting" by
    # the server; this commonly indicates that the mailbox contains
    # new messages.
    MARKED = :Marked

    # Flag indicating that the mailbox does not contains new messages.
    UNMARKED = :Unmarked

    # Returns the debug mode.
    def self.debug
      return @@debug
    end

    # Sets the debug mode.
    def self.debug=(val)
      return @@debug = val
    end

    # Returns the max number of flags interned to symbols.
    def self.max_flag_count
      return @@max_flag_count
    end

    # Sets the max number of flags interned to symbols.
    def self.max_flag_count=(count)
      @@max_flag_count = count
    end

    # Adds an authenticator for Net::IMAP#authenticate.  +auth_type+
    # is the type of authentication this authenticator supports
    # (for instance, "LOGIN").  The +authenticator+ is an object
    # which defines a process() method to handle authentication with
    # the server.  See Net::IMAP::LoginAuthenticator,
    # Net::IMAP::CramMD5Authenticator, and Net::IMAP::DigestMD5Authenticator
    # for examples.
    #
    #
    # If +auth_type+ refers to an existing authenticator, it will be
    # replaced by the new one.
    def self.add_authenticator(auth_type, authenticator)
      @@authenticators[auth_type] = authenticator
    end

    # The default port for IMAP connections, port 143
    def self.default_port
      return PORT
    end
    
    # The default port for IMAPS connections, port 993
    def self.default_tls_port
      return SSL_PORT
    end

    class << self
      alias default_imap_port default_port
      alias default_imaps_port default_tls_port
      alias default_ssl_port default_tls_port
    end

    # Disconnects from the server.
    def disconnect
      begin
        begin
          # try to call SSL::SSLSocket#io.
          @sock.io.shutdown
        rescue NoMethodError
          # @sock is not an SSL::SSLSocket.
          @sock.shutdown
        end
      rescue Errno::ENOTCONN
        # ignore `Errno::ENOTCONN: Socket is not connected' on some platforms.
      rescue Exception => e
        @receiver_thread.raise(e)
      end
      @receiver_thread.join
      synchronize do
        unless @sock.closed?
          @sock.close
        end
      end
      raise e if e
    end

    # Returns true if disconnected from the server.
    def disconnected?
      return @sock.closed?
    end

    # Sends a CAPABILITY command, and returns an array of
    # capabilities that the server supports.  Each capability
    # is a string.  See [IMAP] for a list of possible
    # capabilities.
    #
    # Note that the Net::IMAP class does not modify its
    # behaviour according to the capabilities of the server;
    # it is up to the user of the class to ensure that
    # a certain capability is supported by a server before
    # using it.
    def capability
      synchronize do
        send_command("CAPABILITY")
        return @responses.delete("CAPABILITY")[-1]
      end
    end

    # Sends a NOOP command to the server. It does nothing.
    def noop
      send_command("NOOP")
    end

    # Sends a LOGOUT command to inform the server that the client is
    # done with the connection.
    def logout
      send_command("LOGOUT")
    end

    # Sends a STARTTLS command to start TLS session.
    def starttls(options = {}, verify = true)
      send_command("STARTTLS") do |resp|
        if resp.kind_of?(TaggedResponse) && resp.name == "OK"
          begin
            # for backward compatibility
            certs = options.to_str
            options = create_ssl_params(certs, verify)
          rescue NoMethodError
          end
          start_tls_session(options)
        end
      end
    end

    # Sends an AUTHENTICATE command to authenticate the client.
    # The +auth_type+ parameter is a string that represents
    # the authentication mechanism to be used. Currently Net::IMAP
    # supports authentication mechanisms:
    #
    #   LOGIN:: login using cleartext user and password.
    #   CRAM-MD5:: login with cleartext user and encrypted password
    #              (see [RFC-2195] for a full description).  This
    #              mechanism requires that the server have the user's
    #              password stored in clear-text password.
    #
    # For both these mechanisms, there should be two +args+: username
    # and (cleartext) password.  A server may not support one or other
    # of these mechanisms; check #capability() for a capability of
    # the form "AUTH=LOGIN" or "AUTH=CRAM-MD5".
    #
    # Authentication is done using the appropriate authenticator object:
    # see @@authenticators for more information on plugging in your own
    # authenticator.
    #
    # For example:
    #
    #    imap.authenticate('LOGIN', user, password)
    #
    # A Net::IMAP::NoResponseError is raised if authentication fails.
    def authenticate(auth_type, *args)
      auth_type = auth_type.upcase
      unless @@authenticators.has_key?(auth_type)
        raise ArgumentError,
          format('unknown auth type - "%s"', auth_type)
      end
      authenticator = @@authenticators[auth_type].new(*args)
      send_command("AUTHENTICATE", auth_type) do |resp|
        if resp.instance_of?(ContinuationRequest)
          data = authenticator.process(resp.data.text.unpack("m")[0])
          s = [data].pack("m").gsub(/\n/, "")
          send_string_data(s)
          put_string(CRLF)
        end
      end
    end

    # Sends a LOGIN command to identify the client and carries
    # the plaintext +password+ authenticating this +user+.  Note
    # that, unlike calling #authenticate() with an +auth_type+
    # of "LOGIN", #login() does *not* use the login authenticator.
    #
    # A Net::IMAP::NoResponseError is raised if authentication fails.
    def login(user, password)
      send_command("LOGIN", user, password)
    end

    # Sends a SELECT command to select a +mailbox+ so that messages
    # in the +mailbox+ can be accessed.
    #
    # After you have selected a mailbox, you may retrieve the
    # number of items in that mailbox from @responses["EXISTS"][-1],
    # and the number of recent messages from @responses["RECENT"][-1].
    # Note that these values can change if new messages arrive
    # during a session; see #add_response_handler() for a way of
    # detecting this event.
    #
    # A Net::IMAP::NoResponseError is raised if the mailbox does not
    # exist or is for some reason non-selectable.
    def select(mailbox)
      synchronize do
        @responses.clear
        send_command("SELECT", mailbox)
      end
    end

    # Sends a EXAMINE command to select a +mailbox+ so that messages
    # in the +mailbox+ can be accessed.  Behaves the same as #select(),
    # except that the selected +mailbox+ is identified as read-only.
    #
    # A Net::IMAP::NoResponseError is raised if the mailbox does not
    # exist or is for some reason non-examinable.
    def examine(mailbox)
      synchronize do
        @responses.clear
        send_command("EXAMINE", mailbox)
      end
    end

    # Sends a CREATE command to create a new +mailbox+.
    #
    # A Net::IMAP::NoResponseError is raised if a mailbox with that name
    # cannot be created.
    def create(mailbox)
      send_command("CREATE", mailbox)
    end

    # Sends a DELETE command to remove the +mailbox+.
    #
    # A Net::IMAP::NoResponseError is raised if a mailbox with that name
    # cannot be deleted, either because it does not exist or because the
    # client does not have permission to delete it.
    def delete(mailbox)
      send_command("DELETE", mailbox)
    end

    # Sends a RENAME command to change the name of the +mailbox+ to
    # +newname+.
    #
    # A Net::IMAP::NoResponseError is raised if a mailbox with the
    # name +mailbox+ cannot be renamed to +newname+ for whatever
    # reason; for instance, because +mailbox+ does not exist, or
    # because there is already a mailbox with the name +newname+.
    def rename(mailbox, newname)
      send_command("RENAME", mailbox, newname)
    end

    # Sends a SUBSCRIBE command to add the specified +mailbox+ name to
    # the server's set of "active" or "subscribed" mailboxes as returned
    # by #lsub().
    #
    # A Net::IMAP::NoResponseError is raised if +mailbox+ cannot be
    # subscribed to, for instance because it does not exist.
    def subscribe(mailbox)
      send_command("SUBSCRIBE", mailbox)
    end

    # Sends a UNSUBSCRIBE command to remove the specified +mailbox+ name
    # from the server's set of "active" or "subscribed" mailboxes.
    #
    # A Net::IMAP::NoResponseError is raised if +mailbox+ cannot be
    # unsubscribed from, for instance because the client is not currently
    # subscribed to it.
    def unsubscribe(mailbox)
      send_command("UNSUBSCRIBE", mailbox)
    end

    # Sends a LIST command, and returns a subset of names from
    # the complete set of all names available to the client.
    # +refname+ provides a context (for instance, a base directory
    # in a directory-based mailbox hierarchy).  +mailbox+ specifies
    # a mailbox or (via wildcards) mailboxes under that context.
    # Two wildcards may be used in +mailbox+: '*', which matches
    # all characters *including* the hierarchy delimiter (for instance,
    # '/' on a UNIX-hosted directory-based mailbox hierarchy); and '%',
    # which matches all characters *except* the hierarchy delimiter.
    #
    # If +refname+ is empty, +mailbox+ is used directly to determine
    # which mailboxes to match.  If +mailbox+ is empty, the root
    # name of +refname+ and the hierarchy delimiter are returned.
    #
    # The return value is an array of +Net::IMAP::MailboxList+. For example:
    #
    #   imap.create("foo/bar")
    #   imap.create("foo/baz")
    #   p imap.list("", "foo/%")
    #   #=> [#<Net::IMAP::MailboxList attr=[:Noselect], delim="/", name="foo/">, \\
    #        #<Net::IMAP::MailboxList attr=[:Noinferiors, :Marked], delim="/", name="foo/bar">, \\
    #        #<Net::IMAP::MailboxList attr=[:Noinferiors], delim="/", name="foo/baz">]
    def list(refname, mailbox)
      synchronize do
        send_command("LIST", refname, mailbox)
        return @responses.delete("LIST")
      end
    end

    # Sends a XLIST command, and returns a subset of names from
    # the complete set of all names available to the client.
    # +refname+ provides a context (for instance, a base directory
    # in a directory-based mailbox hierarchy).  +mailbox+ specifies
    # a mailbox or (via wildcards) mailboxes under that context.
    # Two wildcards may be used in +mailbox+: '*', which matches
    # all characters *including* the hierarchy delimiter (for instance,
    # '/' on a UNIX-hosted directory-based mailbox hierarchy); and '%',
    # which matches all characters *except* the hierarchy delimiter.
    #
    # If +refname+ is empty, +mailbox+ is used directly to determine
    # which mailboxes to match.  If +mailbox+ is empty, the root
    # name of +refname+ and the hierarchy delimiter are returned.
    #
    # The XLIST command is like the LIST command except that the flags
    # returned refer to the function of the folder/mailbox, e.g. :Sent
    #
    # The return value is an array of +Net::IMAP::MailboxList+. For example:
    #
    #   imap.create("foo/bar")
    #   imap.create("foo/baz")
    #   p imap.xlist("", "foo/%")
    #   #=> [#<Net::IMAP::MailboxList attr=[:Noselect], delim="/", name="foo/">, \\
    #        #<Net::IMAP::MailboxList attr=[:Noinferiors, :Marked], delim="/", name="foo/bar">, \\
    #        #<Net::IMAP::MailboxList attr=[:Noinferiors], delim="/", name="foo/baz">]
    def xlist(refname, mailbox)
      synchronize do
        send_command("XLIST", refname, mailbox)
        return @responses.delete("XLIST")
      end
    end

    # Sends the GETQUOTAROOT command along with specified +mailbox+.
    # This command is generally available to both admin and user.
    # If mailbox exists, returns an array containing objects of
    # Net::IMAP::MailboxQuotaRoot and Net::IMAP::MailboxQuota.
    def getquotaroot(mailbox)
      synchronize do
        send_command("GETQUOTAROOT", mailbox)
        result = []
        result.concat(@responses.delete("QUOTAROOT"))
        result.concat(@responses.delete("QUOTA"))
        return result
      end
    end

    # Sends the GETQUOTA command along with specified +mailbox+.
    # If this mailbox exists, then an array containing a
    # Net::IMAP::MailboxQuota object is returned.  This
    # command generally is only available to server admin.
    def getquota(mailbox)
      synchronize do
        send_command("GETQUOTA", mailbox)
        return @responses.delete("QUOTA")
      end
    end

    # Sends a SETQUOTA command along with the specified +mailbox+ and
    # +quota+.  If +quota+ is nil, then quota will be unset for that
    # mailbox.  Typically one needs to be logged in as server admin
    # for this to work.  The IMAP quota commands are described in
    # [RFC-2087].
    def setquota(mailbox, quota)
      if quota.nil?
        data = '()'
      else
        data = '(STORAGE ' + quota.to_s + ')'
      end
      send_command("SETQUOTA", mailbox, RawData.new(data))
    end

    # Sends the SETACL command along with +mailbox+, +user+ and the
    # +rights+ that user is to have on that mailbox.  If +rights+ is nil,
    # then that user will be stripped of any rights to that mailbox.
    # The IMAP ACL commands are described in [RFC-2086].
    def setacl(mailbox, user, rights)
      if rights.nil?
        send_command("SETACL", mailbox, user, "")
      else
        send_command("SETACL", mailbox, user, rights)
      end
    end

    # Send the GETACL command along with specified +mailbox+.
    # If this mailbox exists, an array containing objects of
    # Net::IMAP::MailboxACLItem will be returned.
    def getacl(mailbox)
      synchronize do
        send_command("GETACL", mailbox)
        return @responses.delete("ACL")[-1]
      end
    end

    # Sends a LSUB command, and returns a subset of names from the set
    # of names that the user has declared as being "active" or
    # "subscribed".  +refname+ and +mailbox+ are interpreted as
    # for #list().
    # The return value is an array of +Net::IMAP::MailboxList+.
    def lsub(refname, mailbox)
      synchronize do
        send_command("LSUB", refname, mailbox)
        return @responses.delete("LSUB")
      end
    end

    # Sends a STATUS command, and returns the status of the indicated
    # +mailbox+. +attr+ is a list of one or more attributes that
    # we are request the status of.  Supported attributes include:
    #
    #   MESSAGES:: the number of messages in the mailbox.
    #   RECENT:: the number of recent messages in the mailbox.
    #   UNSEEN:: the number of unseen messages in the mailbox.
    #
    # The return value is a hash of attributes. For example:
    #
    #   p imap.status("inbox", ["MESSAGES", "RECENT"])
    #   #=> {"RECENT"=>0, "MESSAGES"=>44}
    #
    # A Net::IMAP::NoResponseError is raised if status values
    # for +mailbox+ cannot be returned, for instance because it
    # does not exist.
    def status(mailbox, attr)
      synchronize do
        send_command("STATUS", mailbox, attr)
        return @responses.delete("STATUS")[-1].attr
      end
    end

    # Sends a APPEND command to append the +message+ to the end of
    # the +mailbox+. The optional +flags+ argument is an array of
    # flags to initially passing to the new message.  The optional
    # +date_time+ argument specifies the creation time to assign to the
    # new message; it defaults to the current time.
    # For example:
    #
    #   imap.append("inbox", <<EOF.gsub(/\n/, "\r\n"), [:Seen], Time.now)
    #   Subject: hello
    #   From: shugo@ruby-lang.org
    #   To: shugo@ruby-lang.org
    #
    #   hello world
    #   EOF
    #
    # A Net::IMAP::NoResponseError is raised if the mailbox does
    # not exist (it is not created automatically), or if the flags,
    # date_time, or message arguments contain errors.
    def append(mailbox, message, flags = nil, date_time = nil)
      args = []
      if flags
        args.push(flags)
      end
      args.push(date_time) if date_time
      args.push(Literal.new(message))
      send_command("APPEND", mailbox, *args)
    end

    # Sends a CHECK command to request a checkpoint of the currently
    # selected mailbox.  This performs implementation-specific
    # housekeeping, for instance, reconciling the mailbox's
    # in-memory and on-disk state.
    def check
      send_command("CHECK")
    end

    # Sends a CLOSE command to close the currently selected mailbox.
    # The CLOSE command permanently removes from the mailbox all
    # messages that have the \Deleted flag set.
    def close
      send_command("CLOSE")
    end

    # Sends a EXPUNGE command to permanently remove from the currently
    # selected mailbox all messages that have the \Deleted flag set.
    def expunge
      synchronize do
        send_command("EXPUNGE")
        return @responses.delete("EXPUNGE")
      end
    end

    # Sends a SEARCH command to search the mailbox for messages that
    # match the given searching criteria, and returns message sequence
    # numbers.  +keys+ can either be a string holding the entire
    # search string, or a single-dimension array of search keywords and
    # arguments.  The following are some common search criteria;
    # see [IMAP] section 6.4.4 for a full list.
    #
    # <message set>:: a set of message sequence numbers.  ',' indicates
    #                 an interval, ':' indicates a range.  For instance,
    #                 '2,10:12,15' means "2,10,11,12,15".
    #
    # BEFORE <date>:: messages with an internal date strictly before
    #                 <date>.  The date argument has a format similar
    #                 to 8-Aug-2002.
    #
    # BODY <string>:: messages that contain <string> within their body.
    #
    # CC <string>:: messages containing <string> in their CC field.
    #
    # FROM <string>:: messages that contain <string> in their FROM field.
    #
    # NEW:: messages with the \Recent, but not the \Seen, flag set.
    #
    # NOT <search-key>:: negate the following search key.
    #
    # OR <search-key> <search-key>:: "or" two search keys together.
    #
    # ON <date>:: messages with an internal date exactly equal to <date>,
    #             which has a format similar to 8-Aug-2002.
    #
    # SINCE <date>:: messages with an internal date on or after <date>.
    #
    # SUBJECT <string>:: messages with <string> in their subject.
    #
    # TO <string>:: messages with <string> in their TO field.
    #
    # For example:
    #
    #   p imap.search(["SUBJECT", "hello", "NOT", "NEW"])
    #   #=> [1, 6, 7, 8]
    def search(keys, charset = nil)
      return search_internal("SEARCH", keys, charset)
    end

    # As for #search(), but returns unique identifiers.
    def uid_search(keys, charset = nil)
      return search_internal("UID SEARCH", keys, charset)
    end

    # Sends a FETCH command to retrieve data associated with a message
    # in the mailbox. The +set+ parameter is a number or an array of
    # numbers or a Range object. The number is a message sequence
    # number.  +attr+ is a list of attributes to fetch; see the
    # documentation for Net::IMAP::FetchData for a list of valid
    # attributes.
    # The return value is an array of Net::IMAP::FetchData. For example:
    #
    #   p imap.fetch(6..8, "UID")
    #   #=> [#<Net::IMAP::FetchData seqno=6, attr={"UID"=>98}>, \\
    #        #<Net::IMAP::FetchData seqno=7, attr={"UID"=>99}>, \\
    #        #<Net::IMAP::FetchData seqno=8, attr={"UID"=>100}>]
    #   p imap.fetch(6, "BODY[HEADER.FIELDS (SUBJECT)]")
    #   #=> [#<Net::IMAP::FetchData seqno=6, attr={"BODY[HEADER.FIELDS (SUBJECT)]"=>"Subject: test\r\n\r\n"}>]
    #   data = imap.uid_fetch(98, ["RFC822.SIZE", "INTERNALDATE"])[0]
    #   p data.seqno
    #   #=> 6
    #   p data.attr["RFC822.SIZE"]
    #   #=> 611
    #   p data.attr["INTERNALDATE"]
    #   #=> "12-Oct-2000 22:40:59 +0900"
    #   p data.attr["UID"]
    #   #=> 98
    def fetch(set, attr)
      return fetch_internal("FETCH", set, attr)
    end

    # As for #fetch(), but +set+ contains unique identifiers.
    def uid_fetch(set, attr)
      return fetch_internal("UID FETCH", set, attr)
    end

    # Sends a STORE command to alter data associated with messages
    # in the mailbox, in particular their flags. The +set+ parameter
    # is a number or an array of numbers or a Range object. Each number
    # is a message sequence number.  +attr+ is the name of a data item
    # to store: 'FLAGS' means to replace the message's flag list
    # with the provided one; '+FLAGS' means to add the provided flags;
    # and '-FLAGS' means to remove them.  +flags+ is a list of flags.
    #
    # The return value is an array of Net::IMAP::FetchData. For example:
    #
    #   p imap.store(6..8, "+FLAGS", [:Deleted])
    #   #=> [#<Net::IMAP::FetchData seqno=6, attr={"FLAGS"=>[:Seen, :Deleted]}>, \\
    #        #<Net::IMAP::FetchData seqno=7, attr={"FLAGS"=>[:Seen, :Deleted]}>, \\
    #        #<Net::IMAP::FetchData seqno=8, attr={"FLAGS"=>[:Seen, :Deleted]}>]
    def store(set, attr, flags)
      return store_internal("STORE", set, attr, flags)
    end

    # As for #store(), but +set+ contains unique identifiers.
    def uid_store(set, attr, flags)
      return store_internal("UID STORE", set, attr, flags)
    end

    # Sends a COPY command to copy the specified message(s) to the end
    # of the specified destination +mailbox+. The +set+ parameter is
    # a number or an array of numbers or a Range object. The number is
    # a message sequence number.
    def copy(set, mailbox)
      copy_internal("COPY", set, mailbox)
    end

    # As for #copy(), but +set+ contains unique identifiers.
    def uid_copy(set, mailbox)
      copy_internal("UID COPY", set, mailbox)
    end

    # Sends a SORT command to sort messages in the mailbox.
    # Returns an array of message sequence numbers. For example:
    #
    #   p imap.sort(["FROM"], ["ALL"], "US-ASCII")
    #   #=> [1, 2, 3, 5, 6, 7, 8, 4, 9]
    #   p imap.sort(["DATE"], ["SUBJECT", "hello"], "US-ASCII")
    #   #=> [6, 7, 8, 1]
    #
    # See [SORT-THREAD-EXT] for more details.
    def sort(sort_keys, search_keys, charset)
      return sort_internal("SORT", sort_keys, search_keys, charset)
    end

    # As for #sort(), but returns an array of unique identifiers.
    def uid_sort(sort_keys, search_keys, charset)
      return sort_internal("UID SORT", sort_keys, search_keys, charset)
    end

    # Adds a response handler. For example, to detect when
    # the server sends us a new EXISTS response (which normally
    # indicates new messages being added to the mail box),
    # you could add the following handler after selecting the
    # mailbox.
    #
    #   imap.add_response_handler { |resp|
    #     if resp.kind_of?(Net::IMAP::UntaggedResponse) and resp.name == "EXISTS"
    #       puts "Mailbox now has #{resp.data} messages"
    #     end
    #   }
    #
    def add_response_handler(handler = Proc.new)
      @response_handlers.push(handler)
    end

    # Removes the response handler.
    def remove_response_handler(handler)
      @response_handlers.delete(handler)
    end

    # As for #search(), but returns message sequence numbers in threaded
    # format, as a Net::IMAP::ThreadMember tree.  The supported algorithms
    # are:
    #
    # ORDEREDSUBJECT:: split into single-level threads according to subject,
    #                  ordered by date.
    # REFERENCES:: split into threads by parent/child relationships determined
    #              by which message is a reply to which.
    #
    # Unlike #search(), +charset+ is a required argument.  US-ASCII
    # and UTF-8 are sample values.
    #
    # See [SORT-THREAD-EXT] for more details.
    def thread(algorithm, search_keys, charset)
      return thread_internal("THREAD", algorithm, search_keys, charset)
    end

    # As for #thread(), but returns unique identifiers instead of
    # message sequence numbers.
    def uid_thread(algorithm, search_keys, charset)
      return thread_internal("UID THREAD", algorithm, search_keys, charset)
    end

    # Sends an IDLE command that waits for notifications of new or expunged
    # messages.  Yields responses from the server during the IDLE.
    #
    # Use #idle_done() to leave IDLE.
    def idle(&response_handler)
      raise LocalJumpError, "no block given" unless response_handler

      response = nil

      synchronize do
        tag = Thread.current[:net_imap_tag] = generate_tag
        put_string("#{tag} IDLE#{CRLF}")

        begin
          add_response_handler(response_handler)
          @idle_done_cond = new_cond
          @idle_done_cond.wait
          @idle_done_cond = nil
          if @receiver_thread_terminating
            raise Net::IMAP::Error, "connection closed"
          end
        ensure
          unless @receiver_thread_terminating
            remove_response_handler(response_handler)
            put_string("DONE#{CRLF}")
            response = get_tagged_response(tag, "IDLE")
          end
        end
      end

      return response
    end

    # Leaves IDLE.
    def idle_done
      synchronize do
        if @idle_done_cond.nil?
          raise Net::IMAP::Error, "not during IDLE"
        end
        @idle_done_cond.signal
      end
    end

    # Decode a string from modified UTF-7 format to UTF-8.
    #
    # UTF-7 is a 7-bit encoding of Unicode [UTF7].  IMAP uses a
    # slightly modified version of this to encode mailbox names
    # containing non-ASCII characters; see [IMAP] section 5.1.3.
    #
    # Net::IMAP does _not_ automatically encode and decode
    # mailbox names to and from utf7.
    def self.decode_utf7(s)
      return s.gsub(/&(.*?)-/n) {
        if $1.empty?
          "&"
        else
          base64 = $1.tr(",", "/")
          x = base64.length % 4
          if x > 0
            base64.concat("=" * (4 - x))
          end
          base64.unpack("m")[0].unpack("n*").pack("U*")
        end
      }.force_encoding("UTF-8")
    end

    # Encode a string from UTF-8 format to modified UTF-7.
    def self.encode_utf7(s)
      return s.gsub(/(&)|([^\x20-\x7e]+)/u) {
        if $1
          "&-"
        else
          base64 = [$&.unpack("U*").pack("n*")].pack("m")
          "&" + base64.delete("=\n").tr("/", ",") + "-"
        end
      }.force_encoding("ASCII-8BIT")
    end

    # Formats +time+ as an IMAP-style date.
    def self.format_date(time)
      return time.strftime('%d-%b-%Y')
    end

    # Formats +time+ as an IMAP-style date-time.
    def self.format_datetime(time)
      return time.strftime('%d-%b-%Y %H:%M %z')
    end

    private

    CRLF = "\r\n"      # :nodoc:
    PORT = 143         # :nodoc:
    SSL_PORT = 993   # :nodoc:

    @@debug = false
    @@authenticators = {}
    @@max_flag_count = 10000

    # :call-seq:
    #    Net::IMAP.new(host, options = {})
    #
    # Creates a new Net::IMAP object and connects it to the specified
    # +host+.
    #
    # +options+ is an option hash, each key of which is a symbol.
    #
    # The available options are:
    #
    # port::  port number (default value is 143 for imap, or 993 for imaps)
    # ssl::   if options[:ssl] is true, then an attempt will be made
    #         to use SSL (now TLS) to connect to the server.  For this to work
    #         OpenSSL [OSSL] and the Ruby OpenSSL [RSSL] extensions need to
    #         be installed.
    #         if options[:ssl] is a hash, it's passed to
    #         OpenSSL::SSL::SSLContext#set_params as parameters.
    #
    # The most common errors are:
    #
    # Errno::ECONNREFUSED:: connection refused by +host+ or an intervening
    #                       firewall.
    # Errno::ETIMEDOUT:: connection timed out (possibly due to packets
    #                    being dropped by an intervening firewall).
    # Errno::ENETUNREACH:: there is no route to that network.
    # SocketError:: hostname not known or other socket error.
    # Net::IMAP::ByeResponseError:: we connected to the host, but they
    #                               immediately said goodbye to us.
    def initialize(host, port_or_options = {},
                   usessl = false, certs = nil, verify = true)
      super()
      @host = host
      begin
        options = port_or_options.to_hash
      rescue NoMethodError
        # for backward compatibility
        options = {}
        options[:port] = port_or_options
        if usessl
          options[:ssl] = create_ssl_params(certs, verify)
        end
      end
      @port = options[:port] || (options[:ssl] ? SSL_PORT : PORT)
      @tag_prefix = "RUBY"
      @tagno = 0
      @parser = ResponseParser.new
      @sock = TCPSocket.open(@host, @port)
      if options[:ssl]
        start_tls_session(options[:ssl])
        @usessl = true
      else
        @usessl = false
      end
      @responses = Hash.new([].freeze)
      @tagged_responses = {}
      @response_handlers = []
      @tagged_response_arrival = new_cond
      @continuation_request_arrival = new_cond
      @idle_done_cond = nil
      @logout_command_tag = nil
      @debug_output_bol = true
      @exception = nil

      @greeting = get_response
      if @greeting.nil?
        @sock.close
        raise Error, "connection closed"
      end
      if @greeting.name == "BYE"
        @sock.close
        raise ByeResponseError, @greeting
      end

      @client_thread = Thread.current
      @receiver_thread = Thread.start {
        begin
          receive_responses
        rescue Exception
        end
      }
      @receiver_thread_terminating = false
    end

    def receive_responses
      connection_closed = false
      until connection_closed
        synchronize do
          @exception = nil
        end
        begin
          resp = get_response
        rescue Exception => e
          synchronize do
            @sock.close
            @exception = e
          end
          break
        end
        unless resp
          synchronize do
            @exception = EOFError.new("end of file reached")
          end
          break
        end
        begin
          synchronize do
            case resp
            when TaggedResponse
              @tagged_responses[resp.tag] = resp
              @tagged_response_arrival.broadcast
              if resp.tag == @logout_command_tag
                return
              end
            when UntaggedResponse
              record_response(resp.name, resp.data)
              if resp.data.instance_of?(ResponseText) &&
                  (code = resp.data.code)
                record_response(code.name, code.data)
              end
              if resp.name == "BYE" && @logout_command_tag.nil?
                @sock.close
                @exception = ByeResponseError.new(resp)
                connection_closed = true
              end
            when ContinuationRequest
              @continuation_request_arrival.signal
            end
            @response_handlers.each do |handler|
              handler.call(resp)
            end
          end
        rescue Exception => e
          @exception = e
          synchronize do
            @tagged_response_arrival.broadcast
            @continuation_request_arrival.broadcast
          end
        end
      end
      synchronize do
        @receiver_thread_terminating = true
        @tagged_response_arrival.broadcast
        @continuation_request_arrival.broadcast
        if @idle_done_cond
          @idle_done_cond.signal 
        end
      end
    end

    def get_tagged_response(tag, cmd)
      until @tagged_responses.key?(tag)
        raise @exception if @exception
        @tagged_response_arrival.wait
      end
      resp = @tagged_responses.delete(tag)
      case resp.name
      when /\A(?:NO)\z/ni
        raise NoResponseError, resp
      when /\A(?:BAD)\z/ni
        raise BadResponseError, resp
      else
        return resp
      end
    end

    def get_response
      buff = ""
      while true
        s = @sock.gets(CRLF)
        break unless s
        buff.concat(s)
        if /\{(\d+)\}\r\n/n =~ s
          s = @sock.read($1.to_i)
          buff.concat(s)
        else
          break
        end
      end
      return nil if buff.length == 0
      if @@debug
        $stderr.print(buff.gsub(/^/n, "S: "))
      end
      return @parser.parse(buff)
    end

    def record_response(name, data)
      unless @responses.has_key?(name)
        @responses[name] = []
      end
      @responses[name].push(data)
    end

    def send_command(cmd, *args, &block)
      synchronize do
        args.each do |i|
          validate_data(i)
        end
        tag = generate_tag
        put_string(tag + " " + cmd)
        args.each do |i|
          put_string(" ")
          send_data(i)
        end
        put_string(CRLF)
        if cmd == "LOGOUT"
          @logout_command_tag = tag
        end
        if block
          add_response_handler(block)
        end
        begin
          return get_tagged_response(tag, cmd)
        ensure
          if block
            remove_response_handler(block)
          end
        end
      end
    end

    def generate_tag
      @tagno += 1
      return format("%s%04d", @tag_prefix, @tagno)
    end

    def put_string(str)
      @sock.print(str)
      if @@debug
        if @debug_output_bol
          $stderr.print("C: ")
        end
        $stderr.print(str.gsub(/\n(?!\z)/n, "\nC: "))
        if /\r\n\z/n.match(str)
          @debug_output_bol = true
        else
          @debug_output_bol = false
        end
      end
    end

    def validate_data(data)
      case data
      when nil
      when String
      when Integer
        if data < 0 || data >= 4294967296
          raise DataFormatError, num.to_s
        end
      when Array
        data.each do |i|
          validate_data(i)
        end
      when Time
      when Symbol
      else
        data.validate
      end
    end

    def send_data(data)
      case data
      when nil
        put_string("NIL")
      when String
        send_string_data(data)
      when Integer
        send_number_data(data)
      when Array
        send_list_data(data)
      when Time
        send_time_data(data)
      when Symbol
        send_symbol_data(data)
      else
        data.send_data(self)
      end
    end

    def send_string_data(str)
      case str
      when ""
        put_string('""')
      when /[\x80-\xff\r\n]/n
        # literal
        send_literal(str)
      when /[(){ \x00-\x1f\x7f%*"\\]/n
        # quoted string
        send_quoted_string(str)
      else
        put_string(str)
      end
    end

    def send_quoted_string(str)
      put_string('"' + str.gsub(/["\\]/n, "\\\\\\&") + '"')
    end

    def send_literal(str)
      put_string("{" + str.bytesize.to_s + "}" + CRLF)
      @continuation_request_arrival.wait
      raise @exception if @exception
      put_string(str)
    end

    def send_number_data(num)
      put_string(num.to_s)
    end

    def send_list_data(list)
      put_string("(")
      first = true
      list.each do |i|
        if first
          first = false
        else
          put_string(" ")
        end
        send_data(i)
      end
      put_string(")")
    end

    DATE_MONTH = %w(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec)

    def send_time_data(time)
      t = time.dup.gmtime
      s = format('"%2d-%3s-%4d %02d:%02d:%02d +0000"',
                 t.day, DATE_MONTH[t.month - 1], t.year,
                 t.hour, t.min, t.sec)
      put_string(s)
    end

    def send_symbol_data(symbol)
      put_string("\\" + symbol.to_s)
    end

    def search_internal(cmd, keys, charset)
      if keys.instance_of?(String)
        keys = [RawData.new(keys)]
      else
        normalize_searching_criteria(keys)
      end
      synchronize do
        if charset
          send_command(cmd, "CHARSET", charset, *keys)
        else
          send_command(cmd, *keys)
        end
        return @responses.delete("SEARCH")[-1]
      end
    end

    def fetch_internal(cmd, set, attr)
      case attr
      when String then
        attr = RawData.new(attr)
      when Array then
        attr = attr.map { |arg|
          arg.is_a?(String) ? RawData.new(arg) : arg
        }
      end

      synchronize do
        @responses.delete("FETCH")
        send_command(cmd, MessageSet.new(set), attr)
        return @responses.delete("FETCH")
      end
    end

    def store_internal(cmd, set, attr, flags)
      if attr.instance_of?(String)
        attr = RawData.new(attr)
      end
      synchronize do
        @responses.delete("FETCH")
        send_command(cmd, MessageSet.new(set), attr, flags)
        return @responses.delete("FETCH")
      end
    end

    def copy_internal(cmd, set, mailbox)
      send_command(cmd, MessageSet.new(set), mailbox)
    end

    def sort_internal(cmd, sort_keys, search_keys, charset)
      if search_keys.instance_of?(String)
        search_keys = [RawData.new(search_keys)]
      else
        normalize_searching_criteria(search_keys)
      end
      normalize_searching_criteria(search_keys)
      synchronize do
        send_command(cmd, sort_keys, charset, *search_keys)
        return @responses.delete("SORT")[-1]
      end
    end

    def thread_internal(cmd, algorithm, search_keys, charset)
      if search_keys.instance_of?(String)
        search_keys = [RawData.new(search_keys)]
      else
        normalize_searching_criteria(search_keys)
      end
      normalize_searching_criteria(search_keys)
      send_command(cmd, algorithm, charset, *search_keys)
      return @responses.delete("THREAD")[-1]
    end

    def normalize_searching_criteria(keys)
      keys.collect! do |i|
        case i
        when -1, Range, Array
          MessageSet.new(i)
        else
          i
        end
      end
    end

    def create_ssl_params(certs = nil, verify = true)
      params = {}
      if certs
        if File.file?(certs)
          params[:ca_file] = certs
        elsif File.directory?(certs)
          params[:ca_path] = certs
        end
      end
      if verify
        params[:verify_mode] = VERIFY_PEER
      else
        params[:verify_mode] = VERIFY_NONE
      end
      return params
    end

    def start_tls_session(params = {})
      unless defined?(OpenSSL)
        raise "SSL extension not installed"
      end
      if @sock.kind_of?(OpenSSL::SSL::SSLSocket)
        raise RuntimeError, "already using SSL"
      end
      begin
        params = params.to_hash
      rescue NoMethodError
        params = {}
      end
      context = SSLContext.new
      context.set_params(params)
      if defined?(VerifyCallbackProc)
        context.verify_callback = VerifyCallbackProc
      end
      @sock = SSLSocket.new(@sock, context)
      @sock.sync_close = true
      @sock.connect
      if context.verify_mode != VERIFY_NONE
        @sock.post_connection_check(@host)
      end
    end

    class RawData # :nodoc:
      def send_data(imap)
        imap.send(:put_string, @data)
      end

      def validate
      end

      private

      def initialize(data)
        @data = data
      end
    end

    class Atom # :nodoc:
      def send_data(imap)
        imap.send(:put_string, @data)
      end

      def validate
      end

      private

      def initialize(data)
        @data = data
      end
    end

    class QuotedString # :nodoc:
      def send_data(imap)
        imap.send(:send_quoted_string, @data)
      end

      def validate
      end

      private

      def initialize(data)
        @data = data
      end
    end

    class Literal # :nodoc:
      def send_data(imap)
        imap.send(:send_literal, @data)
      end

      def validate
      end

      private

      def initialize(data)
        @data = data
      end
    end

    class MessageSet # :nodoc:
      def send_data(imap)
        imap.send(:put_string, format_internal(@data))
      end

      def validate
        validate_internal(@data)
      end

      private

      def initialize(data)
        @data = data
      end

      def format_internal(data)
        case data
        when "*"
          return data
        when Integer
          if data == -1
            return "*"
          else
            return data.to_s
          end
        when Range
          return format_internal(data.first) +
            ":" + format_internal(data.last)
        when Array
          return data.collect {|i| format_internal(i)}.join(",")
        when ThreadMember
          return data.seqno.to_s +
            ":" + data.children.collect {|i| format_internal(i).join(",")}
        end
      end

      def validate_internal(data)
        case data
        when "*"
        when Integer
          ensure_nz_number(data)
        when Range
        when Array
          data.each do |i|
            validate_internal(i)
          end
        when ThreadMember
          data.children.each do |i|
            validate_internal(i)
          end
        else
          raise DataFormatError, data.inspect
        end
      end

      def ensure_nz_number(num)
        if num < -1 || num == 0 || num >= 4294967296
          msg = "nz_number must be non-zero unsigned 32-bit integer: " +
                num.inspect
          raise DataFormatError, msg
        end
      end
    end

    # Net::IMAP::ContinuationRequest represents command continuation requests.
    #
    # The command continuation request response is indicated by a "+" token
    # instead of a tag.  This form of response indicates that the server is
    # ready to accept the continuation of a command from the client.  The
    # remainder of this response is a line of text.
    #
    #   continue_req    ::= "+" SPACE (resp_text / base64)
    #
    # ==== Fields:
    #
    # data:: Returns the data (Net::IMAP::ResponseText).
    #
    # raw_data:: Returns the raw data string.
    ContinuationRequest = Struct.new(:data, :raw_data)

    # Net::IMAP::UntaggedResponse represents untagged responses.
    #
    # Data transmitted by the server to the client and status responses
    # that do not indicate command completion are prefixed with the token
    # "*", and are called untagged responses.
    #
    #   response_data   ::= "*" SPACE (resp_cond_state / resp_cond_bye /
    #                       mailbox_data / message_data / capability_data)
    #
    # ==== Fields:
    #
    # name:: Returns the name such as "FLAGS", "LIST", "FETCH"....
    #
    # data:: Returns the data such as an array of flag symbols,
    #         a ((<Net::IMAP::MailboxList>)) object....
    #
    # raw_data:: Returns the raw data string.
    UntaggedResponse = Struct.new(:name, :data, :raw_data)

    # Net::IMAP::TaggedResponse represents tagged responses.
    #
    # The server completion result response indicates the success or
    # failure of the operation.  It is tagged with the same tag as the
    # client command which began the operation.
    #
    #   response_tagged ::= tag SPACE resp_cond_state CRLF
    #
    #   tag             ::= 1*<any ATOM_CHAR except "+">
    #
    #   resp_cond_state ::= ("OK" / "NO" / "BAD") SPACE resp_text
    #
    # ==== Fields:
    #
    # tag:: Returns the tag.
    #
    # name:: Returns the name. the name is one of "OK", "NO", "BAD".
    #
    # data:: Returns the data. See ((<Net::IMAP::ResponseText>)).
    #
    # raw_data:: Returns the raw data string.
    #
    TaggedResponse = Struct.new(:tag, :name, :data, :raw_data)

    # Net::IMAP::ResponseText represents texts of responses.
    # The text may be prefixed by the response code.
    #
    #   resp_text       ::= ["[" resp_text_code "]" SPACE] (text_mime2 / text)
    #                       ;; text SHOULD NOT begin with "[" or "="
    #
    # ==== Fields:
    #
    # code:: Returns the response code. See ((<Net::IMAP::ResponseCode>)).
    #
    # text:: Returns the text.
    #
    ResponseText = Struct.new(:code, :text)

    #
    # Net::IMAP::ResponseCode represents response codes.
    #
    #   resp_text_code  ::= "ALERT" / "PARSE" /
    #                       "PERMANENTFLAGS" SPACE "(" #(flag / "\*") ")" /
    #                       "READ-ONLY" / "READ-WRITE" / "TRYCREATE" /
    #                       "UIDVALIDITY" SPACE nz_number /
    #                       "UNSEEN" SPACE nz_number /
    #                       atom [SPACE 1*<any TEXT_CHAR except "]">]
    #
    # ==== Fields:
    #
    # name:: Returns the name such as "ALERT", "PERMANENTFLAGS", "UIDVALIDITY"....
    #
    # data:: Returns the data if it exists.
    #
    ResponseCode = Struct.new(:name, :data)

    # Net::IMAP::MailboxList represents contents of the LIST response.
    #
    #   mailbox_list    ::= "(" #("\Marked" / "\Noinferiors" /
    #                       "\Noselect" / "\Unmarked" / flag_extension) ")"
    #                       SPACE (<"> QUOTED_CHAR <"> / nil) SPACE mailbox
    #
    # ==== Fields:
    #
    # attr:: Returns the name attributes. Each name attribute is a symbol
    #        capitalized by String#capitalize, such as :Noselect (not :NoSelect).
    #
    # delim:: Returns the hierarchy delimiter
    #
    # name:: Returns the mailbox name.
    #
    MailboxList = Struct.new(:attr, :delim, :name)

    # Net::IMAP::MailboxQuota represents contents of GETQUOTA response.
    # This object can also be a response to GETQUOTAROOT.  In the syntax
    # specification below, the delimiter used with the "#" construct is a
    # single space (SPACE).
    #
    #    quota_list      ::= "(" #quota_resource ")"
    #
    #    quota_resource  ::= atom SPACE number SPACE number
    #
    #    quota_response  ::= "QUOTA" SPACE astring SPACE quota_list
    #
    # ==== Fields:
    #
    # mailbox:: The mailbox with the associated quota.
    #
    # usage:: Current storage usage of mailbox.
    #
    # quota:: Quota limit imposed on mailbox.
    #
    MailboxQuota = Struct.new(:mailbox, :usage, :quota)

    # Net::IMAP::MailboxQuotaRoot represents part of the GETQUOTAROOT
    # response. (GETQUOTAROOT can also return Net::IMAP::MailboxQuota.)
    #
    #    quotaroot_response ::= "QUOTAROOT" SPACE astring *(SPACE astring)
    #
    # ==== Fields:
    #
    # mailbox:: The mailbox with the associated quota.
    #
    # quotaroots:: Zero or more quotaroots that effect the quota on the
    #              specified mailbox.
    #
    MailboxQuotaRoot = Struct.new(:mailbox, :quotaroots)

    # Net::IMAP::MailboxACLItem represents response from GETACL.
    #
    #    acl_data        ::= "ACL" SPACE mailbox *(SPACE identifier SPACE rights)
    #
    #    identifier      ::= astring
    #
    #    rights          ::= astring
    #
    # ==== Fields:
    #
    # user:: Login name that has certain rights to the mailbox
    #        that was specified with the getacl command.
    #
    # rights:: The access rights the indicated user has to the
    #          mailbox.
    #
    MailboxACLItem = Struct.new(:user, :rights)

    # Net::IMAP::StatusData represents contents of the STATUS response.
    #
    # ==== Fields:
    #
    # mailbox:: Returns the mailbox name.
    #
    # attr:: Returns a hash. Each key is one of "MESSAGES", "RECENT", "UIDNEXT",
    #        "UIDVALIDITY", "UNSEEN". Each value is a number.
    #
    StatusData = Struct.new(:mailbox, :attr)

    # Net::IMAP::FetchData represents contents of the FETCH response.
    #
    # ==== Fields:
    #
    # seqno:: Returns the message sequence number.
    #         (Note: not the unique identifier, even for the UID command response.)
    #
    # attr:: Returns a hash. Each key is a data item name, and each value is
    #        its value.
    #
    #        The current data items are:
    #
    #        [BODY]
    #           A form of BODYSTRUCTURE without extension data.
    #        [BODY[<section>]<<origin_octet>>]
    #           A string expressing the body contents of the specified section.
    #        [BODYSTRUCTURE]
    #           An object that describes the [MIME-IMB] body structure of a message.
    #           See Net::IMAP::BodyTypeBasic, Net::IMAP::BodyTypeText,
    #           Net::IMAP::BodyTypeMessage, Net::IMAP::BodyTypeMultipart.
    #        [ENVELOPE]
    #           A Net::IMAP::Envelope object that describes the envelope
    #           structure of a message.
    #        [FLAGS]
    #           A array of flag symbols that are set for this message. flag symbols
    #           are capitalized by String#capitalize.
    #        [INTERNALDATE]
    #           A string representing the internal date of the message.
    #        [RFC822]
    #           Equivalent to BODY[].
    #        [RFC822.HEADER]
    #           Equivalent to BODY.PEEK[HEADER].
    #        [RFC822.SIZE]
    #           A number expressing the [RFC-822] size of the message.
    #        [RFC822.TEXT]
    #           Equivalent to BODY[TEXT].
    #        [UID]
    #           A number expressing the unique identifier of the message.
    #
    FetchData = Struct.new(:seqno, :attr)

    # Net::IMAP::Envelope represents envelope structures of messages.
    #
    # ==== Fields:
    #
    # date:: Returns a string that represents the date.
    #
    # subject:: Returns a string that represents the subject.
    #
    # from:: Returns an array of Net::IMAP::Address that represents the from.
    #
    # sender:: Returns an array of Net::IMAP::Address that represents the sender.
    #
    # reply_to:: Returns an array of Net::IMAP::Address that represents the reply-to.
    #
    # to:: Returns an array of Net::IMAP::Address that represents the to.
    #
    # cc:: Returns an array of Net::IMAP::Address that represents the cc.
    #
    # bcc:: Returns an array of Net::IMAP::Address that represents the bcc.
    #
    # in_reply_to:: Returns a string that represents the in-reply-to.
    #
    # message_id:: Returns a string that represents the message-id.
    #
    Envelope = Struct.new(:date, :subject, :from, :sender, :reply_to,
                          :to, :cc, :bcc, :in_reply_to, :message_id)

    #
    # Net::IMAP::Address represents electronic mail addresses.
    #
    # ==== Fields:
    #
    # name:: Returns the phrase from [RFC-822] mailbox.
    #
    # route:: Returns the route from [RFC-822] route-addr.
    #
    # mailbox:: nil indicates end of [RFC-822] group.
    #           If non-nil and host is nil, returns [RFC-822] group name.
    #           Otherwise, returns [RFC-822] local-part
    #
    # host:: nil indicates [RFC-822] group syntax.
    #        Otherwise, returns [RFC-822] domain name.
    #
    Address = Struct.new(:name, :route, :mailbox, :host)

    #
    # Net::IMAP::ContentDisposition represents Content-Disposition fields.
    #
    # ==== Fields:
    #
    # dsp_type:: Returns the disposition type.
    #
    # param:: Returns a hash that represents parameters of the Content-Disposition
    #         field.
    #
    ContentDisposition = Struct.new(:dsp_type, :param)

    # Net::IMAP::ThreadMember represents a thread-node returned
    # by Net::IMAP#thread
    #
    # ==== Fields:
    #
    # seqno:: The sequence number of this message.
    #
    # children:: an array of Net::IMAP::ThreadMember objects for mail
    # items that are children of this in the thread.
    #
    ThreadMember = Struct.new(:seqno, :children)

    # Net::IMAP::BodyTypeBasic represents basic body structures of messages.
    #
    # ==== Fields:
    #
    # media_type:: Returns the content media type name as defined in [MIME-IMB].
    #
    # subtype:: Returns the content subtype name as defined in [MIME-IMB].
    #
    # param:: Returns a hash that represents parameters as defined in [MIME-IMB].
    #
    # content_id:: Returns a string giving the content id as defined in [MIME-IMB].
    #
    # description:: Returns a string giving the content description as defined in
    #               [MIME-IMB].
    #
    # encoding:: Returns a string giving the content transfer encoding as defined in
    #            [MIME-IMB].
    #
    # size:: Returns a number giving the size of the body in octets.
    #
    # md5:: Returns a string giving the body MD5 value as defined in [MD5].
    #
    # disposition:: Returns a Net::IMAP::ContentDisposition object giving
    #               the content disposition.
    #
    # language:: Returns a string or an array of strings giving the body
    #            language value as defined in [LANGUAGE-TAGS].
    #
    # extension:: Returns extension data.
    #
    # multipart?:: Returns false.
    #
    class BodyTypeBasic < Struct.new(:media_type, :subtype,
                                     :param, :content_id,
                                     :description, :encoding, :size,
                                     :md5, :disposition, :language,
                                     :extension)
      def multipart?
        return false
      end

      # Obsolete: use +subtype+ instead.  Calling this will
      # generate a warning message to +stderr+, then return
      # the value of +subtype+.
      def media_subtype
        $stderr.printf("warning: media_subtype is obsolete.\n")
        $stderr.printf("         use subtype instead.\n")
        return subtype
      end
    end

    # Net::IMAP::BodyTypeText represents TEXT body structures of messages.
    #
    # ==== Fields:
    #
    # lines:: Returns the size of the body in text lines.
    #
    # And Net::IMAP::BodyTypeText has all fields of Net::IMAP::BodyTypeBasic.
    #
    class BodyTypeText < Struct.new(:media_type, :subtype,
                                    :param, :content_id,
                                    :description, :encoding, :size,
                                    :lines,
                                    :md5, :disposition, :language,
                                    :extension)
      def multipart?
        return false
      end

      # Obsolete: use +subtype+ instead.  Calling this will
      # generate a warning message to +stderr+, then return
      # the value of +subtype+.
      def media_subtype
        $stderr.printf("warning: media_subtype is obsolete.\n")
        $stderr.printf("         use subtype instead.\n")
        return subtype
      end
    end

    # Net::IMAP::BodyTypeMessage represents MESSAGE/RFC822 body structures of messages.
    #
    # ==== Fields:
    #
    # envelope:: Returns a Net::IMAP::Envelope giving the envelope structure.
    #
    # body:: Returns an object giving the body structure.
    #
    # And Net::IMAP::BodyTypeMessage has all methods of Net::IMAP::BodyTypeText.
    #
    class BodyTypeMessage < Struct.new(:media_type, :subtype,
                                       :param, :content_id,
                                       :description, :encoding, :size,
                                       :envelope, :body, :lines,
                                       :md5, :disposition, :language,
                                       :extension)
      def multipart?
        return false
      end

      # Obsolete: use +subtype+ instead.  Calling this will
      # generate a warning message to +stderr+, then return
      # the value of +subtype+.
      def media_subtype
        $stderr.printf("warning: media_subtype is obsolete.\n")
        $stderr.printf("         use subtype instead.\n")
        return subtype
      end
    end

    # Net::IMAP::BodyTypeMultipart represents multipart body structures
    # of messages.
    #
    # ==== Fields:
    #
    # media_type:: Returns the content media type name as defined in [MIME-IMB].
    #
    # subtype:: Returns the content subtype name as defined in [MIME-IMB].
    #
    # parts:: Returns multiple parts.
    #
    # param:: Returns a hash that represents parameters as defined in [MIME-IMB].
    #
    # disposition:: Returns a Net::IMAP::ContentDisposition object giving
    #               the content disposition.
    #
    # language:: Returns a string or an array of strings giving the body
    #            language value as defined in [LANGUAGE-TAGS].
    #
    # extension:: Returns extension data.
    #
    # multipart?:: Returns true.
    #
    class BodyTypeMultipart < Struct.new(:media_type, :subtype,
                                         :parts,
                                         :param, :disposition, :language,
                                         :extension)
      def multipart?
        return true
      end

      # Obsolete: use +subtype+ instead.  Calling this will
      # generate a warning message to +stderr+, then return
      # the value of +subtype+.
      def media_subtype
        $stderr.printf("warning: media_subtype is obsolete.\n")
        $stderr.printf("         use subtype instead.\n")
        return subtype
      end
    end

    class ResponseParser # :nodoc:
      def initialize
        @str = nil
        @pos = nil
        @lex_state = nil
        @token = nil
        @flag_symbols = {}
      end

      def parse(str)
        @str = str
        @pos = 0
        @lex_state = EXPR_BEG
        @token = nil
        return response
      end

      private

      EXPR_BEG          = :EXPR_BEG
      EXPR_DATA         = :EXPR_DATA
      EXPR_TEXT         = :EXPR_TEXT
      EXPR_RTEXT        = :EXPR_RTEXT
      EXPR_CTEXT        = :EXPR_CTEXT

      T_SPACE   = :SPACE
      T_NIL     = :NIL
      T_NUMBER  = :NUMBER
      T_ATOM    = :ATOM
      T_QUOTED  = :QUOTED
      T_LPAR    = :LPAR
      T_RPAR    = :RPAR
      T_BSLASH  = :BSLASH
      T_STAR    = :STAR
      T_LBRA    = :LBRA
      T_RBRA    = :RBRA
      T_LITERAL = :LITERAL
      T_PLUS    = :PLUS
      T_PERCENT = :PERCENT
      T_CRLF    = :CRLF
      T_EOF     = :EOF
      T_TEXT    = :TEXT

      BEG_REGEXP = /\G(?:\
(?# 1:  SPACE   )( +)|\
(?# 2:  NIL     )(NIL)(?=[\x80-\xff(){ \x00-\x1f\x7f%*#{'"'}\\\[\]+])|\
(?# 3:  NUMBER  )(\d+)(?=[\x80-\xff(){ \x00-\x1f\x7f%*#{'"'}\\\[\]+])|\
(?# 4:  ATOM    )([^\x80-\xff(){ \x00-\x1f\x7f%*#{'"'}\\\[\]+]+)|\
(?# 5:  QUOTED  )"((?:[^\x00\r\n"\\]|\\["\\])*)"|\
(?# 6:  LPAR    )(\()|\
(?# 7:  RPAR    )(\))|\
(?# 8:  BSLASH  )(\\)|\
(?# 9:  STAR    )(\*)|\
(?# 10: LBRA    )(\[)|\
(?# 11: RBRA    )(\])|\
(?# 12: LITERAL )\{(\d+)\}\r\n|\
(?# 13: PLUS    )(\+)|\
(?# 14: PERCENT )(%)|\
(?# 15: CRLF    )(\r\n)|\
(?# 16: EOF     )(\z))/ni

      DATA_REGEXP = /\G(?:\
(?# 1:  SPACE   )( )|\
(?# 2:  NIL     )(NIL)|\
(?# 3:  NUMBER  )(\d+)|\
(?# 4:  QUOTED  )"((?:[^\x00\r\n"\\]|\\["\\])*)"|\
(?# 5:  LITERAL )\{(\d+)\}\r\n|\
(?# 6:  LPAR    )(\()|\
(?# 7:  RPAR    )(\)))/ni

      TEXT_REGEXP = /\G(?:\
(?# 1:  TEXT    )([^\x00\r\n]*))/ni

      RTEXT_REGEXP = /\G(?:\
(?# 1:  LBRA    )(\[)|\
(?# 2:  TEXT    )([^\x00\r\n]*))/ni

      CTEXT_REGEXP = /\G(?:\
(?# 1:  TEXT    )([^\x00\r\n\]]*))/ni

      Token = Struct.new(:symbol, :value)

      def response
        token = lookahead
        case token.symbol
        when T_PLUS
          result = continue_req
        when T_STAR
          result = response_untagged
        else
          result = response_tagged
        end
        match(T_CRLF)
        match(T_EOF)
        return result
      end

      def continue_req
        match(T_PLUS)
        match(T_SPACE)
        return ContinuationRequest.new(resp_text, @str)
      end

      def response_untagged
        match(T_STAR)
        match(T_SPACE)
        token = lookahead
        if token.symbol == T_NUMBER
          return numeric_response
        elsif token.symbol == T_ATOM
          case token.value
          when /\A(?:OK|NO|BAD|BYE|PREAUTH)\z/ni
            return response_cond
          when /\A(?:FLAGS)\z/ni
            return flags_response
          when /\A(?:LIST|LSUB|XLIST)\z/ni
            return list_response
          when /\A(?:QUOTA)\z/ni
            return getquota_response
          when /\A(?:QUOTAROOT)\z/ni
            return getquotaroot_response
          when /\A(?:ACL)\z/ni
            return getacl_response
          when /\A(?:SEARCH|SORT)\z/ni
            return search_response
          when /\A(?:THREAD)\z/ni
            return thread_response
          when /\A(?:STATUS)\z/ni
            return status_response
          when /\A(?:CAPABILITY)\z/ni
            return capability_response
          else
            return text_response
          end
        else
          parse_error("unexpected token %s", token.symbol)
        end
      end

      def response_tagged
        tag = atom
        match(T_SPACE)
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return TaggedResponse.new(tag, name, resp_text, @str)
      end

      def response_cond
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return UntaggedResponse.new(name, resp_text, @str)
      end

      def numeric_response
        n = number
        match(T_SPACE)
        token = match(T_ATOM)
        name = token.value.upcase
        case name
        when "EXISTS", "RECENT", "EXPUNGE"
          return UntaggedResponse.new(name, n, @str)
        when "FETCH"
          shift_token
          match(T_SPACE)
          data = FetchData.new(n, msg_att(n))
          return UntaggedResponse.new(name, data, @str)
        end
      end

      def msg_att(n)
        match(T_LPAR)
        attr = {}
        while true
          token = lookahead
          case token.symbol
          when T_RPAR
            shift_token
            break
          when T_SPACE
            shift_token
            next
          end
          case token.value
          when /\A(?:ENVELOPE)\z/ni
            name, val = envelope_data
          when /\A(?:FLAGS)\z/ni
            name, val = flags_data
          when /\A(?:INTERNALDATE)\z/ni
            name, val = internaldate_data
          when /\A(?:RFC822(?:\.HEADER|\.TEXT)?)\z/ni
            name, val = rfc822_text
          when /\A(?:RFC822\.SIZE)\z/ni
            name, val = rfc822_size
          when /\A(?:BODY(?:STRUCTURE)?)\z/ni
            name, val = body_data
          when /\A(?:UID)\z/ni
            name, val = uid_data
          else
            parse_error("unknown attribute `%s' for {%d}", token.value, n)
          end
          attr[name] = val
        end
        return attr
      end

      def envelope_data
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return name, envelope
      end

      def envelope
        @lex_state = EXPR_DATA
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          result = nil
        else
          match(T_LPAR)
          date = nstring
          match(T_SPACE)
          subject = nstring
          match(T_SPACE)
          from = address_list
          match(T_SPACE)
          sender = address_list
          match(T_SPACE)
          reply_to = address_list
          match(T_SPACE)
          to = address_list
          match(T_SPACE)
          cc = address_list
          match(T_SPACE)
          bcc = address_list
          match(T_SPACE)
          in_reply_to = nstring
          match(T_SPACE)
          message_id = nstring
          match(T_RPAR)
          result = Envelope.new(date, subject, from, sender, reply_to,
                                to, cc, bcc, in_reply_to, message_id)
        end
        @lex_state = EXPR_BEG
        return result
      end

      def flags_data
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return name, flag_list
      end

      def internaldate_data
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        token = match(T_QUOTED)
        return name, token.value
      end

      def rfc822_text
        token = match(T_ATOM)
        name = token.value.upcase
        token = lookahead
        if token.symbol == T_LBRA
          shift_token
          match(T_RBRA)
        end
        match(T_SPACE)
        return name, nstring
      end

      def rfc822_size
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return name, number
      end

      def body_data
        token = match(T_ATOM)
        name = token.value.upcase
        token = lookahead
        if token.symbol == T_SPACE
          shift_token
          return name, body
        end
        name.concat(section)
        token = lookahead
        if token.symbol == T_ATOM
          name.concat(token.value)
          shift_token
        end
        match(T_SPACE)
        data = nstring
        return name, data
      end

      def body
        @lex_state = EXPR_DATA
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          result = nil
        else
          match(T_LPAR)
          token = lookahead
          if token.symbol == T_LPAR
            result = body_type_mpart
          else
            result = body_type_1part
          end
          match(T_RPAR)
        end
        @lex_state = EXPR_BEG
        return result
      end

      def body_type_1part
        token = lookahead
        case token.value
        when /\A(?:TEXT)\z/ni
          return body_type_text
        when /\A(?:MESSAGE)\z/ni
          return body_type_msg
        else
          return body_type_basic
        end
      end

      def body_type_basic
        mtype, msubtype = media_type
        token = lookahead
        if token.symbol == T_RPAR
          return BodyTypeBasic.new(mtype, msubtype)
        end
        match(T_SPACE)
        param, content_id, desc, enc, size = body_fields
        md5, disposition, language, extension = body_ext_1part
        return BodyTypeBasic.new(mtype, msubtype,
                                 param, content_id,
                                 desc, enc, size,
                                 md5, disposition, language, extension)
      end

      def body_type_text
        mtype, msubtype = media_type
        match(T_SPACE)
        param, content_id, desc, enc, size = body_fields
        match(T_SPACE)
        lines = number
        md5, disposition, language, extension = body_ext_1part
        return BodyTypeText.new(mtype, msubtype,
                                param, content_id,
                                desc, enc, size,
                                lines,
                                md5, disposition, language, extension)
      end

      def body_type_msg
        mtype, msubtype = media_type
        match(T_SPACE)
        param, content_id, desc, enc, size = body_fields
        match(T_SPACE)
        env = envelope
        match(T_SPACE)
        b = body
        match(T_SPACE)
        lines = number
        md5, disposition, language, extension = body_ext_1part
        return BodyTypeMessage.new(mtype, msubtype,
                                   param, content_id,
                                   desc, enc, size,
                                   env, b, lines,
                                   md5, disposition, language, extension)
      end

      def body_type_mpart
        parts = []
        while true
          token = lookahead
          if token.symbol == T_SPACE
            shift_token
            break
          end
          parts.push(body)
        end
        mtype = "MULTIPART"
        msubtype = case_insensitive_string
        param, disposition, language, extension = body_ext_mpart
        return BodyTypeMultipart.new(mtype, msubtype, parts,
                                     param, disposition, language,
                                     extension)
      end

      def media_type
        mtype = case_insensitive_string
        match(T_SPACE)
        msubtype = case_insensitive_string
        return mtype, msubtype
      end

      def body_fields
        param = body_fld_param
        match(T_SPACE)
        content_id = nstring
        match(T_SPACE)
        desc = nstring
        match(T_SPACE)
        enc = case_insensitive_string
        match(T_SPACE)
        size = number
        return param, content_id, desc, enc, size
      end

      def body_fld_param
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        end
        match(T_LPAR)
        param = {}
        while true
          token = lookahead
          case token.symbol
          when T_RPAR
            shift_token
            break
          when T_SPACE
            shift_token
          end
          name = case_insensitive_string
          match(T_SPACE)
          val = string
          param[name] = val
        end
        return param
      end

      def body_ext_1part
        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return nil
        end
        md5 = nstring

        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return md5
        end
        disposition = body_fld_dsp

        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return md5, disposition
        end
        language = body_fld_lang

        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return md5, disposition, language
        end

        extension = body_extensions
        return md5, disposition, language, extension
      end

      def body_ext_mpart
        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return nil
        end
        param = body_fld_param

        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return param
        end
        disposition = body_fld_dsp
        match(T_SPACE)
        language = body_fld_lang

        token = lookahead
        if token.symbol == T_SPACE
          shift_token
        else
          return param, disposition, language
        end

        extension = body_extensions
        return param, disposition, language, extension
      end

      def body_fld_dsp
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        end
        match(T_LPAR)
        dsp_type = case_insensitive_string
        match(T_SPACE)
        param = body_fld_param
        match(T_RPAR)
        return ContentDisposition.new(dsp_type, param)
      end

      def body_fld_lang
        token = lookahead
        if token.symbol == T_LPAR
          shift_token
          result = []
          while true
            token = lookahead
            case token.symbol
            when T_RPAR
              shift_token
              return result
            when T_SPACE
              shift_token
            end
            result.push(case_insensitive_string)
          end
        else
          lang = nstring
          if lang
            return lang.upcase
          else
            return lang
          end
        end
      end

      def body_extensions
        result = []
        while true
          token = lookahead
          case token.symbol
          when T_RPAR
            return result
          when T_SPACE
            shift_token
          end
          result.push(body_extension)
        end
      end

      def body_extension
        token = lookahead
        case token.symbol
        when T_LPAR
          shift_token
          result = body_extensions
          match(T_RPAR)
          return result
        when T_NUMBER
          return number
        else
          return nstring
        end
      end

      def section
        str = ""
        token = match(T_LBRA)
        str.concat(token.value)
        token = match(T_ATOM, T_NUMBER, T_RBRA)
        if token.symbol == T_RBRA
          str.concat(token.value)
          return str
        end
        str.concat(token.value)
        token = lookahead
        if token.symbol == T_SPACE
          shift_token
          str.concat(token.value)
          token = match(T_LPAR)
          str.concat(token.value)
          while true
            token = lookahead
            case token.symbol
            when T_RPAR
              str.concat(token.value)
              shift_token
              break
            when T_SPACE
              shift_token
              str.concat(token.value)
            end
            str.concat(format_string(astring))
          end
        end
        token = match(T_RBRA)
        str.concat(token.value)
        return str
      end

      def format_string(str)
        case str
        when ""
          return '""'
        when /[\x80-\xff\r\n]/n
          # literal
          return "{" + str.bytesize.to_s + "}" + CRLF + str
        when /[(){ \x00-\x1f\x7f%*"\\]/n
          # quoted string
          return '"' + str.gsub(/["\\]/n, "\\\\\\&") + '"'
        else
          # atom
          return str
        end
      end

      def uid_data
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return name, number
      end

      def text_response
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        @lex_state = EXPR_TEXT
        token = match(T_TEXT)
        @lex_state = EXPR_BEG
        return UntaggedResponse.new(name, token.value)
      end

      def flags_response
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return UntaggedResponse.new(name, flag_list, @str)
      end

      def list_response
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        return UntaggedResponse.new(name, mailbox_list, @str)
      end

      def mailbox_list
        attr = flag_list
        match(T_SPACE)
        token = match(T_QUOTED, T_NIL)
        if token.symbol == T_NIL
          delim = nil
        else
          delim = token.value
        end
        match(T_SPACE)
        name = astring
        return MailboxList.new(attr, delim, name)
      end

      def getquota_response
        # If quota never established, get back
        # `NO Quota root does not exist'.
        # If quota removed, get `()' after the
        # folder spec with no mention of `STORAGE'.
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        mailbox = astring
        match(T_SPACE)
        match(T_LPAR)
        token = lookahead
        case token.symbol
        when T_RPAR
          shift_token
          data = MailboxQuota.new(mailbox, nil, nil)
          return UntaggedResponse.new(name, data, @str)
        when T_ATOM
          shift_token
          match(T_SPACE)
          token = match(T_NUMBER)
          usage = token.value
          match(T_SPACE)
          token = match(T_NUMBER)
          quota = token.value
          match(T_RPAR)
          data = MailboxQuota.new(mailbox, usage, quota)
          return UntaggedResponse.new(name, data, @str)
        else
          parse_error("unexpected token %s", token.symbol)
        end
      end

      def getquotaroot_response
        # Similar to getquota, but only admin can use getquota.
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        mailbox = astring
        quotaroots = []
        while true
          token = lookahead
          break unless token.symbol == T_SPACE
          shift_token
          quotaroots.push(astring)
        end
        data = MailboxQuotaRoot.new(mailbox, quotaroots)
        return UntaggedResponse.new(name, data, @str)
      end

      def getacl_response
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        data = []
        token = lookahead
        if token.symbol == T_SPACE
          shift_token
          while true
            token = lookahead
            case token.symbol
            when T_CRLF
              break
            when T_SPACE
              shift_token
            end
            user = astring
            match(T_SPACE)
            rights = astring
            ##XXX data.push([user, rights])
            data.push(MailboxACLItem.new(user, rights))
          end
        end
        return UntaggedResponse.new(name, data, @str)
      end

      def search_response
        token = match(T_ATOM)
        name = token.value.upcase
        token = lookahead
        if token.symbol == T_SPACE
          shift_token
          data = []
          while true
            token = lookahead
            case token.symbol
            when T_CRLF
              break
            when T_SPACE
              shift_token
            else
              data.push(number)
            end
          end
        else
          data = []
        end
        return UntaggedResponse.new(name, data, @str)
      end

      def thread_response
        token = match(T_ATOM)
        name = token.value.upcase
        token = lookahead

        if token.symbol == T_SPACE
          threads = []

          while true
            shift_token
            token = lookahead

            case token.symbol
            when T_LPAR
              threads << thread_branch(token)
            when T_CRLF
              break
            end
          end
        else
          # no member
          threads = []
        end

        return UntaggedResponse.new(name, threads, @str)
      end

      def thread_branch(token)
        rootmember = nil
        lastmember = nil

        while true
          shift_token    # ignore first T_LPAR
          token = lookahead

          case token.symbol
          when T_NUMBER
            # new member
            newmember = ThreadMember.new(number, [])
            if rootmember.nil?
              rootmember = newmember
            else
              lastmember.children << newmember
            end
            lastmember = newmember
          when T_SPACE
            # do nothing
          when T_LPAR
            if rootmember.nil?
              # dummy member
              lastmember = rootmember = ThreadMember.new(nil, [])
            end

            lastmember.children << thread_branch(token)
          when T_RPAR
            break
          end
        end

        return rootmember
      end

      def status_response
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        mailbox = astring
        match(T_SPACE)
        match(T_LPAR)
        attr = {}
        while true
          token = lookahead
          case token.symbol
          when T_RPAR
            shift_token
            break
          when T_SPACE
            shift_token
          end
          token = match(T_ATOM)
          key = token.value.upcase
          match(T_SPACE)
          val = number
          attr[key] = val
        end
        data = StatusData.new(mailbox, attr)
        return UntaggedResponse.new(name, data, @str)
      end

      def capability_response
        token = match(T_ATOM)
        name = token.value.upcase
        match(T_SPACE)
        data = []
        while true
          token = lookahead
          case token.symbol
          when T_CRLF
            break
          when T_SPACE
            shift_token
          end
          data.push(atom.upcase)
        end
        return UntaggedResponse.new(name, data, @str)
      end

      def resp_text
        @lex_state = EXPR_RTEXT
        token = lookahead
        if token.symbol == T_LBRA
          code = resp_text_code
        else
          code = nil
        end
        token = match(T_TEXT)
        @lex_state = EXPR_BEG
        return ResponseText.new(code, token.value)
      end

      def resp_text_code
        @lex_state = EXPR_BEG
        match(T_LBRA)
        token = match(T_ATOM)
        name = token.value.upcase
        case name
        when /\A(?:ALERT|PARSE|READ-ONLY|READ-WRITE|TRYCREATE|NOMODSEQ)\z/n
          result = ResponseCode.new(name, nil)
        when /\A(?:PERMANENTFLAGS)\z/n
          match(T_SPACE)
          result = ResponseCode.new(name, flag_list)
        when /\A(?:UIDVALIDITY|UIDNEXT|UNSEEN)\z/n
          match(T_SPACE)
          result = ResponseCode.new(name, number)
        else
          token = lookahead
          if token.symbol == T_SPACE
            shift_token
            @lex_state = EXPR_CTEXT
            token = match(T_TEXT)
            @lex_state = EXPR_BEG
            result = ResponseCode.new(name, token.value)
          else
            result = ResponseCode.new(name, nil)
          end
        end
        match(T_RBRA)
        @lex_state = EXPR_RTEXT
        return result
      end

      def address_list
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        else
          result = []
          match(T_LPAR)
          while true
            token = lookahead
            case token.symbol
            when T_RPAR
              shift_token
              break
            when T_SPACE
              shift_token
            end
            result.push(address)
          end
          return result
        end
      end

      ADDRESS_REGEXP = /\G\
(?# 1: NAME     )(?:NIL|"((?:[^\x80-\xff\x00\r\n"\\]|\\["\\])*)") \
(?# 2: ROUTE    )(?:NIL|"((?:[^\x80-\xff\x00\r\n"\\]|\\["\\])*)") \
(?# 3: MAILBOX  )(?:NIL|"((?:[^\x80-\xff\x00\r\n"\\]|\\["\\])*)") \
(?# 4: HOST     )(?:NIL|"((?:[^\x80-\xff\x00\r\n"\\]|\\["\\])*)")\
\)/ni

      def address
        match(T_LPAR)
        if @str.index(ADDRESS_REGEXP, @pos)
          # address does not include literal.
          @pos = $~.end(0)
          name = $1
          route = $2
          mailbox = $3
          host = $4
          for s in [name, route, mailbox, host]
            if s
              s.gsub!(/\\(["\\])/n, "\\1")
            end
          end
        else
          name = nstring
          match(T_SPACE)
          route = nstring
          match(T_SPACE)
          mailbox = nstring
          match(T_SPACE)
          host = nstring
          match(T_RPAR)
        end
        return Address.new(name, route, mailbox, host)
      end

#        def flag_list
#       result = []
#       match(T_LPAR)
#       while true
#         token = lookahead
#         case token.symbol
#         when T_RPAR
#           shift_token
#           break
#         when T_SPACE
#           shift_token
#         end
#         result.push(flag)
#       end
#       return result
#        end

#        def flag
#       token = lookahead
#       if token.symbol == T_BSLASH
#         shift_token
#         token = lookahead
#         if token.symbol == T_STAR
#           shift_token
#           return token.value.intern
#         else
#           return atom.intern
#         end
#       else
#         return atom
#       end
#        end

      FLAG_REGEXP = /\
(?# FLAG        )\\([^\x80-\xff(){ \x00-\x1f\x7f%"\\]+)|\
(?# ATOM        )([^\x80-\xff(){ \x00-\x1f\x7f%*"\\]+)/n

      def flag_list
        if @str.index(/\(([^)]*)\)/ni, @pos)
          @pos = $~.end(0)
          return $1.scan(FLAG_REGEXP).collect { |flag, atom|
            if atom
              atom
            else
              symbol = flag.capitalize.untaint.intern
              @flag_symbols[symbol] = true
              if @flag_symbols.length > IMAP.max_flag_count
                raise FlagCountError, "number of flag symbols exceeded"
              end
              symbol
            end
          }
        else
          parse_error("invalid flag list")
        end
      end

      def nstring
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        else
          return string
        end
      end

      def astring
        token = lookahead
        if string_token?(token)
          return string
        else
          return atom
        end
      end

      def string
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        end
        token = match(T_QUOTED, T_LITERAL)
        return token.value
      end

      STRING_TOKENS = [T_QUOTED, T_LITERAL, T_NIL]

      def string_token?(token)
        return STRING_TOKENS.include?(token.symbol)
      end

      def case_insensitive_string
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        end
        token = match(T_QUOTED, T_LITERAL)
        return token.value.upcase
      end

      def atom
        result = ""
        while true
          token = lookahead
          if atom_token?(token)
            result.concat(token.value)
            shift_token
          else
            if result.empty?
              parse_error("unexpected token %s", token.symbol)
            else
              return result
            end
          end
        end
      end

      ATOM_TOKENS = [
        T_ATOM,
        T_NUMBER,
        T_NIL,
        T_LBRA,
        T_RBRA,
        T_PLUS
      ]

      def atom_token?(token)
        return ATOM_TOKENS.include?(token.symbol)
      end

      def number
        token = lookahead
        if token.symbol == T_NIL
          shift_token
          return nil
        end
        token = match(T_NUMBER)
        return token.value.to_i
      end

      def nil_atom
        match(T_NIL)
        return nil
      end

      def match(*args)
        token = lookahead
        unless args.include?(token.symbol)
          parse_error('unexpected token %s (expected %s)',
                      token.symbol.id2name,
                      args.collect {|i| i.id2name}.join(" or "))
        end
        shift_token
        return token
      end

      def lookahead
        unless @token
          @token = next_token
        end
        return @token
      end

      def shift_token
        @token = nil
      end

      def next_token
        case @lex_state
        when EXPR_BEG
          if @str.index(BEG_REGEXP, @pos)
            @pos = $~.end(0)
            if $1
              return Token.new(T_SPACE, $+)
            elsif $2
              return Token.new(T_NIL, $+)
            elsif $3
              return Token.new(T_NUMBER, $+)
            elsif $4
              return Token.new(T_ATOM, $+)
            elsif $5
              return Token.new(T_QUOTED,
                               $+.gsub(/\\(["\\])/n, "\\1"))
            elsif $6
              return Token.new(T_LPAR, $+)
            elsif $7
              return Token.new(T_RPAR, $+)
            elsif $8
              return Token.new(T_BSLASH, $+)
            elsif $9
              return Token.new(T_STAR, $+)
            elsif $10
              return Token.new(T_LBRA, $+)
            elsif $11
              return Token.new(T_RBRA, $+)
            elsif $12
              len = $+.to_i
              val = @str[@pos, len]
              @pos += len
              return Token.new(T_LITERAL, val)
            elsif $13
              return Token.new(T_PLUS, $+)
            elsif $14
              return Token.new(T_PERCENT, $+)
            elsif $15
              return Token.new(T_CRLF, $+)
            elsif $16
              return Token.new(T_EOF, $+)
            else
              parse_error("[Net::IMAP BUG] BEG_REGEXP is invalid")
            end
          else
            @str.index(/\S*/n, @pos)
            parse_error("unknown token - %s", $&.dump)
          end
        when EXPR_DATA
          if @str.index(DATA_REGEXP, @pos)
            @pos = $~.end(0)
            if $1
              return Token.new(T_SPACE, $+)
            elsif $2
              return Token.new(T_NIL, $+)
            elsif $3
              return Token.new(T_NUMBER, $+)
            elsif $4
              return Token.new(T_QUOTED,
                               $+.gsub(/\\(["\\])/n, "\\1"))
            elsif $5
              len = $+.to_i
              val = @str[@pos, len]
              @pos += len
              return Token.new(T_LITERAL, val)
            elsif $6
              return Token.new(T_LPAR, $+)
            elsif $7
              return Token.new(T_RPAR, $+)
            else
              parse_error("[Net::IMAP BUG] DATA_REGEXP is invalid")
            end
          else
            @str.index(/\S*/n, @pos)
            parse_error("unknown token - %s", $&.dump)
          end
        when EXPR_TEXT
          if @str.index(TEXT_REGEXP, @pos)
            @pos = $~.end(0)
            if $1
              return Token.new(T_TEXT, $+)
            else
              parse_error("[Net::IMAP BUG] TEXT_REGEXP is invalid")
            end
          else
            @str.index(/\S*/n, @pos)
            parse_error("unknown token - %s", $&.dump)
          end
        when EXPR_RTEXT
          if @str.index(RTEXT_REGEXP, @pos)
            @pos = $~.end(0)
            if $1
              return Token.new(T_LBRA, $+)
            elsif $2
              return Token.new(T_TEXT, $+)
            else
              parse_error("[Net::IMAP BUG] RTEXT_REGEXP is invalid")
            end
          else
            @str.index(/\S*/n, @pos)
            parse_error("unknown token - %s", $&.dump)
          end
        when EXPR_CTEXT
          if @str.index(CTEXT_REGEXP, @pos)
            @pos = $~.end(0)
            if $1
              return Token.new(T_TEXT, $+)
            else
              parse_error("[Net::IMAP BUG] CTEXT_REGEXP is invalid")
            end
          else
            @str.index(/\S*/n, @pos) #/
            parse_error("unknown token - %s", $&.dump)
          end
        else
          parse_error("invalid @lex_state - %s", @lex_state.inspect)
        end
      end

      def parse_error(fmt, *args)
        if IMAP.debug
          $stderr.printf("@str: %s\n", @str.dump)
          $stderr.printf("@pos: %d\n", @pos)
          $stderr.printf("@lex_state: %s\n", @lex_state)
          if @token
            $stderr.printf("@token.symbol: %s\n", @token.symbol)
            $stderr.printf("@token.value: %s\n", @token.value.inspect)
          end
        end
        raise ResponseParseError, format(fmt, *args)
      end
    end

    # Authenticator for the "LOGIN" authentication type.  See
    # #authenticate().
    class LoginAuthenticator
      def process(data)
        case @state
        when STATE_USER
          @state = STATE_PASSWORD
          return @user
        when STATE_PASSWORD
          return @password
        end
      end

      private

      STATE_USER = :USER
      STATE_PASSWORD = :PASSWORD

      def initialize(user, password)
        @user = user
        @password = password
        @state = STATE_USER
      end
    end
    add_authenticator "LOGIN", LoginAuthenticator

    # Authenticator for the "PLAIN" authentication type.  See
    # #authenticate().
    class PlainAuthenticator
      def process(data)
        return "\0#{@user}\0#{@password}"
      end

      private

      def initialize(user, password)
        @user = user
        @password = password
      end
    end
    add_authenticator "PLAIN", PlainAuthenticator

    # Authenticator for the "CRAM-MD5" authentication type.  See
    # #authenticate().
    class CramMD5Authenticator
      def process(challenge)
        digest = hmac_md5(challenge, @password)
        return @user + " " + digest
      end

      private

      def initialize(user, password)
        @user = user
        @password = password
      end

      def hmac_md5(text, key)
        if key.length > 64
          key = Digest::MD5.digest(key)
        end

        k_ipad = key + "\0" * (64 - key.length)
        k_opad = key + "\0" * (64 - key.length)
        for i in 0..63
          k_ipad[i] = (k_ipad[i].ord ^ 0x36).chr
          k_opad[i] = (k_opad[i].ord ^ 0x5c).chr
        end

        digest = Digest::MD5.digest(k_ipad + text)

        return Digest::MD5.hexdigest(k_opad + digest)
      end
    end
    add_authenticator "CRAM-MD5", CramMD5Authenticator

    # Authenticator for the "DIGEST-MD5" authentication type.  See
    # #authenticate().
    class DigestMD5Authenticator
      def process(challenge)
        case @stage
        when STAGE_ONE
          @stage = STAGE_TWO
          sparams = {}
          c = StringScanner.new(challenge)
          while c.scan(/(?:\s*,)?\s*(\w+)=("(?:[^\\"]+|\\.)*"|[^,]+)\s*/)
            k, v = c[1], c[2]
            if v =~ /^"(.*)"$/
              v = $1
              if v =~ /,/
                v = v.split(',')
              end
            end
            sparams[k] = v
          end

          raise DataFormatError, "Bad Challenge: '#{challenge}'" unless c.rest.size == 0
          raise Error, "Server does not support auth (qop = #{sparams['qop'].join(',')})" unless sparams['qop'].include?("auth")

          response = {
            :nonce => sparams['nonce'],
            :username => @user,
            :realm => sparams['realm'],
            :cnonce => Digest::MD5.hexdigest("%.15f:%.15f:%d" % [Time.now.to_f, rand, Process.pid.to_s]),
            :'digest-uri' => 'imap/' + sparams['realm'],
            :qop => 'auth',
            :maxbuf => 65535,
            :nc => "%08d" % nc(sparams['nonce']),
            :charset => sparams['charset'],
          }

          response[:authzid] = @authname unless @authname.nil?

          # now, the real thing
          a0 = Digest::MD5.digest( [ response.values_at(:username, :realm), @password ].join(':') )

          a1 = [ a0, response.values_at(:nonce,:cnonce) ].join(':')
          a1 << ':' + response[:authzid] unless response[:authzid].nil?

          a2 = "AUTHENTICATE:" + response[:'digest-uri']
          a2 << ":00000000000000000000000000000000" if response[:qop] and response[:qop] =~ /^auth-(?:conf|int)$/

          response[:response] = Digest::MD5.hexdigest(
            [
             Digest::MD5.hexdigest(a1),
             response.values_at(:nonce, :nc, :cnonce, :qop),
             Digest::MD5.hexdigest(a2)
            ].join(':')
          )

          return response.keys.map {|key| qdval(key.to_s, response[key]) }.join(',')
        when STAGE_TWO
          @stage = nil
          # if at the second stage, return an empty string
          if challenge =~ /rspauth=/
            return ''
          else
            raise ResponseParseError, challenge
          end
        else
          raise ResponseParseError, challenge
        end
      end

      def initialize(user, password, authname = nil)
        @user, @password, @authname = user, password, authname
        @nc, @stage = {}, STAGE_ONE
      end

      private

      STAGE_ONE = :stage_one
      STAGE_TWO = :stage_two

      def nc(nonce)
        if @nc.has_key? nonce
          @nc[nonce] = @nc[nonce] + 1
        else
          @nc[nonce] = 1
        end
        return @nc[nonce]
      end

      # some responses need quoting
      def qdval(k, v)
        return if k.nil? or v.nil?
        if %w"username authzid realm nonce cnonce digest-uri qop".include? k
          v.gsub!(/([\\"])/, "\\\1")
          return '%s="%s"' % [k, v]
        else
          return '%s=%s' % [k, v]
        end
      end
    end
    add_authenticator "DIGEST-MD5", DigestMD5Authenticator

    # Superclass of IMAP errors.
    class Error < StandardError
    end

    # Error raised when data is in the incorrect format.
    class DataFormatError < Error
    end

    # Error raised when a response from the server is non-parseable.
    class ResponseParseError < Error
    end

    # Superclass of all errors used to encapsulate "fail" responses
    # from the server.
    class ResponseError < Error

      # The response that caused this error
      attr_accessor :response

      def initialize(response)
        @response = response

        super @response.data.text
      end

    end

    # Error raised upon a "NO" response from the server, indicating
    # that the client command could not be completed successfully.
    class NoResponseError < ResponseError
    end

    # Error raised upon a "BAD" response from the server, indicating
    # that the client command violated the IMAP protocol, or an internal
    # server failure has occurred.
    class BadResponseError < ResponseError
    end

    # Error raised upon a "BYE" response from the server, indicating
    # that the client is not being allowed to login, or has been timed
    # out due to inactivity.
    class ByeResponseError < ResponseError
    end

    # Error raised when too many flags are interned to symbols.
    class FlagCountError < Error
    end
  end
end

if __FILE__ == $0
  # :enddoc:
  require "getoptlong"

  $stdout.sync = true
  $port = nil
  $user = ENV["USER"] || ENV["LOGNAME"]
  $auth = "login"
  $ssl = false
  $starttls = false

  def usage
    <<EOF
usage: #{$0} [options] <host>

  --help                        print this message
  --port=PORT                   specifies port
  --user=USER                   specifies user
  --auth=AUTH                   specifies auth type
  --starttls                    use starttls
  --ssl                         use ssl
EOF
  end

  begin
    require 'io/console'
  rescue LoadError
    def _noecho(&block)
      system("stty", "-echo")
      begin
        yield STDIN
      ensure
        system("stty", "echo")
      end
    end
  else
    def _noecho(&block)
      STDIN.noecho(&block)
    end
  end

  def get_password
    print "password: "
    begin
      return _noecho(&:gets).chomp
    ensure
      puts
    end
  end

  def get_command
    printf("%s@%s> ", $user, $host)
    if line = gets
      return line.strip.split(/\s+/)
    else
      return nil
    end
  end

  parser = GetoptLong.new
  parser.set_options(['--debug', GetoptLong::NO_ARGUMENT],
                     ['--help', GetoptLong::NO_ARGUMENT],
                     ['--port', GetoptLong::REQUIRED_ARGUMENT],
                     ['--user', GetoptLong::REQUIRED_ARGUMENT],
                     ['--auth', GetoptLong::REQUIRED_ARGUMENT],
                     ['--starttls', GetoptLong::NO_ARGUMENT],
                     ['--ssl', GetoptLong::NO_ARGUMENT])
  begin
    parser.each_option do |name, arg|
      case name
      when "--port"
        $port = arg
      when "--user"
        $user = arg
      when "--auth"
        $auth = arg
      when "--ssl"
        $ssl = true
      when "--starttls"
        $starttls = true
      when "--debug"
        Net::IMAP.debug = true
      when "--help"
        usage
        exit
      end
    end
  rescue
    abort usage
  end

  $host = ARGV.shift
  unless $host
    abort usage
  end

  imap = Net::IMAP.new($host, :port => $port, :ssl => $ssl)
  begin
    imap.starttls if $starttls
    class << password = method(:get_password)
      alias to_str call
    end
    imap.authenticate($auth, $user, password)
    while true
      cmd, *args = get_command
      break unless cmd
      begin
        case cmd
        when "list"
          for mbox in imap.list("", args[0] || "*")
            if mbox.attr.include?(Net::IMAP::NOSELECT)
              prefix = "!"
            elsif mbox.attr.include?(Net::IMAP::MARKED)
              prefix = "*"
            else
              prefix = " "
            end
            print prefix, mbox.name, "\n"
          end
        when "select"
          imap.select(args[0] || "inbox")
          print "ok\n"
        when "close"
          imap.close
          print "ok\n"
        when "summary"
          unless messages = imap.responses["EXISTS"][-1]
            puts "not selected"
            next
          end
          if messages > 0
            for data in imap.fetch(1..-1, ["ENVELOPE"])
              print data.seqno, ": ", data.attr["ENVELOPE"].subject, "\n"
            end
          else
            puts "no message"
          end
        when "fetch"
          if args[0]
            data = imap.fetch(args[0].to_i, ["RFC822.HEADER", "RFC822.TEXT"])[0]
            puts data.attr["RFC822.HEADER"]
            puts data.attr["RFC822.TEXT"]
          else
            puts "missing argument"
          end
        when "logout", "exit", "quit"
          break
        when "help", "?"
          print <<EOF
list [pattern]                  list mailboxes
select [mailbox]                select mailbox
close                           close mailbox
summary                         display summary
fetch [msgno]                   display message
logout                          logout
help, ?                         display help message
EOF
        else
          print "unknown command: ", cmd, "\n"
        end
      rescue Net::IMAP::Error
        puts $!
      end
    end
  ensure
    imap.logout
    imap.disconnect
  end
end

