#
# = win32/sspi.rb
#
# Copyright (c) 2006-2007 Justin Bailey
#
# Written and maintained by Justin Bailey <jgbailey@gmail.com>.
#
# This program is free software. You can re-distribute and/or
# modify this program under the same terms of ruby itself ---
# Ruby Distribution License or GNU General Public License.
#

require 'Win32API'

# Implements bindings to Win32 SSPI functions, focused on authentication to a proxy server over HTTP.
module Win32
	module SSPI
		# Specifies how credential structure requested will be used. Only SECPKG_CRED_OUTBOUND is used
		# here.
		SECPKG_CRED_INBOUND = 0x00000001
		SECPKG_CRED_OUTBOUND = 0x00000002
		SECPKG_CRED_BOTH = 0x00000003

		# Format of token. NETWORK format is used here.
		SECURITY_NATIVE_DREP = 0x00000010
		SECURITY_NETWORK_DREP = 0x00000000

		# InitializeSecurityContext Requirement flags
		ISC_REQ_REPLAY_DETECT = 0x00000004
		ISC_REQ_SEQUENCE_DETECT = 0x00000008
		ISC_REQ_CONFIDENTIALITY = 0x00000010
		ISC_REQ_USE_SESSION_KEY = 0x00000020
		ISC_REQ_PROMPT_FOR_CREDS = 0x00000040
		ISC_REQ_CONNECTION = 0x00000800

		# Win32 API Functions. Uses Win32API to bind methods to constants contained in class.
		module API
			# Can be called with AcquireCredentialsHandle.call()
			AcquireCredentialsHandle = Win32API.new("secur32", "AcquireCredentialsHandle", 'ppLpppppp', 'L')
			# Can be called with InitializeSecurityContext.call()
			InitializeSecurityContext = Win32API.new("secur32", "InitializeSecurityContext", 'pppLLLpLpppp', 'L')
			# Can be called with DeleteSecurityContext.call()
			DeleteSecurityContext = Win32API.new("secur32", "DeleteSecurityContext", 'P', 'L')
			# Can be called with FreeCredentialsHandle.call()
			FreeCredentialsHandle = Win32API.new("secur32", "FreeCredentialsHandle", 'P', 'L')
		end

		# SecHandle struct
		class SecurityHandle
			def upper
				@struct.unpack("LL")[1]
			end

			def lower
				@struct.unpack("LL")[0]
			end

			def to_p
				@struct ||= "\0" * 8
			end
		end

		# Some familiar aliases for the SecHandle structure
		CredHandle = CtxtHandle = SecurityHandle

		# TimeStamp struct
		class TimeStamp
			attr_reader :struct

			def to_p
				@struct ||= "\0" * 8
			end
		end

		# Creates binary representaiton of a SecBufferDesc structure,
		# including the SecBuffer contained inside.
		class SecurityBuffer

			SECBUFFER_TOKEN = 2   # Security token

			TOKENBUFSIZE = 12288
			SECBUFFER_VERSION = 0

			def initialize(buffer = nil)
				@buffer = buffer || "\0" * TOKENBUFSIZE
				@bufferSize = @buffer.length
				@type = SECBUFFER_TOKEN
			end

			def bufferSize
				unpack
				@bufferSize
			end

			def bufferType
				unpack
				@type
			end

			def token
				unpack
				@buffer
			end

			def to_p
				# Assumption is that when to_p is called we are going to get a packed structure. Therefore,
				# set @unpacked back to nil so we know to unpack when accessors are next accessed.
				@unpacked = nil
				# Assignment of inner structure to variable is very important here. Without it,
				# will not be able to unpack changes to the structure. Alternative, nested unpacks,
				# does not work (i.e. @struct.unpack("LLP12")[2].unpack("LLP12") results in "no associated pointer")
				@sec_buffer ||= [@bufferSize, @type, @buffer].pack("LLP")
				@struct ||= [SECBUFFER_VERSION, 1, @sec_buffer].pack("LLP")
			end

		private

			# Unpacks the SecurityBufferDesc structure into member variables. We
			# only want to do this once per struct, so the struct is deleted
			# after unpacking.
			def unpack
				if ! @unpacked && @sec_buffer && @struct
					@bufferSize, @type = @sec_buffer.unpack("LL")
					@buffer = @sec_buffer.unpack("LLP#{@bufferSize}")[2]
					@struct = nil
					@sec_buffer = nil
					@unpacked = true
				end
			end
		end

		# SEC_WINNT_AUTH_IDENTITY structure
		class Identity
			SEC_WINNT_AUTH_IDENTITY_ANSI = 0x1

			attr_accessor :user, :domain, :password

			def initialize(user = nil, domain = nil, password = nil)
				@user = user
				@domain = domain
				@password = password
				@flags = SEC_WINNT_AUTH_IDENTITY_ANSI
			end

			def to_p
				[@user, @user ? @user.length : 0,
				 @domain, @domain ? @domain.length : 0,
				 @password, @password ? @password.length : 0,
				 @flags].pack("PLPLPLL")
			end
		end

		# Takes a return result from an SSPI function and interprets the value.
		class SSPIResult
			# Good results
			SEC_E_OK = 0x00000000
			SEC_I_CONTINUE_NEEDED = 0x00090312

			# These are generally returned by InitializeSecurityContext
			SEC_E_INSUFFICIENT_MEMORY = 0x80090300
			SEC_E_INTERNAL_ERROR = 0x80090304
			SEC_E_INVALID_HANDLE = 0x80090301
			SEC_E_INVALID_TOKEN = 0x80090308
			SEC_E_LOGON_DENIED = 0x8009030C
			SEC_E_NO_AUTHENTICATING_AUTHORITY = 0x80090311
			SEC_E_NO_CREDENTIALS = 0x8009030E
			SEC_E_TARGET_UNKNOWN = 0x80090303
			SEC_E_UNSUPPORTED_FUNCTION = 0x80090302
			SEC_E_WRONG_PRINCIPAL = 0x80090322

			# These are generally returned by AcquireCredentialsHandle
			SEC_E_NOT_OWNER = 0x80090306
			SEC_E_SECPKG_NOT_FOUND = 0x80090305
			SEC_E_UNKNOWN_CREDENTIALS = 0x8009030D

			@@map = {}
			constants.each { |v| @@map[self.const_get(v.to_s)] = v }

			attr_reader :value

			def initialize(value)
				# convert to unsigned long
				value = [value].pack("L").unpack("L").first
				raise "#{value.to_s(16)} is not a recognized result" unless @@map.has_key? value
				@value = value
			end

			def to_s
				@@map[@value].to_s
			end

			def ok?
				@value == SEC_I_CONTINUE_NEEDED || @value == SEC_E_OK
			end

			def ==(other)
				if other.is_a?(SSPIResult)
					@value == other.value
				elsif other.is_a?(Fixnum)
					@value == @@map[other]
				else
					false
				end
			end
		end

		# Handles "Negotiate" type authentication. Geared towards authenticating with a proxy server over HTTP
		class NegotiateAuth
			attr_accessor :credentials, :context, :contextAttributes, :user, :domain

			# Default request flags for SSPI functions
			REQUEST_FLAGS = ISC_REQ_CONFIDENTIALITY | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONNECTION

			# NTLM tokens start with this header always. Encoding alone adds "==" and newline, so remove those
      B64_TOKEN_PREFIX = ["NTLMSSP"].pack("m").delete("=\n")

			# Given a connection and a request path, performs authentication as the current user and returns
			# the response from a GET request. The connnection should be a Net::HTTP object, and it should
			# have been constructed using the Net::HTTP.Proxy method, but anything that responds to "get" will work.
			# If a user and domain are given, will authenticate as the given user.
			# Returns the response received from the get method (usually Net::HTTPResponse)
			def NegotiateAuth.proxy_auth_get(http, path, user = nil, domain = nil)
				raise "http must respond to :get" unless http.respond_to?(:get)
				nego_auth = self.new user, domain

				resp = http.get path, { "Proxy-Authorization" => "Negotiate " + nego_auth.get_initial_token }
				if resp["Proxy-Authenticate"]
					resp = http.get path, { "Proxy-Authorization" => "Negotiate " + nego_auth.complete_authentication(resp["Proxy-Authenticate"].split(" ").last.strip) }
				end

				resp
			end

			# Creates a new instance ready for authentication as the given user in the given domain.
			# Defaults to current user and domain as defined by ENV["USERDOMAIN"] and ENV["USERNAME"] if
			# no arguments are supplied.
			def initialize(user = nil, domain = nil)
				if user.nil? && domain.nil? && ENV["USERNAME"].nil? && ENV["USERDOMAIN"].nil?
					raise "A username or domain must be supplied since they cannot be retrieved from the environment"
				end

				@user = user || ENV["USERNAME"]
				@domain = domain || ENV["USERDOMAIN"]
			end

			# Gets the initial Negotiate token. Returns it as a base64 encoded string suitable for use in HTTP. Can
			# be easily decoded, however.
			def get_initial_token
				raise "This object is no longer usable because its resources have been freed." if @cleaned_up
				get_credentials

				outputBuffer = SecurityBuffer.new
				@context = CtxtHandle.new
				@contextAttributes = "\0" * 4

				result = SSPIResult.new(API::InitializeSecurityContext.call(@credentials.to_p, nil, nil,
					REQUEST_FLAGS,0, SECURITY_NETWORK_DREP, nil, 0, @context.to_p, outputBuffer.to_p, @contextAttributes, TimeStamp.new.to_p))

				if result.ok? then
					return encode_token(outputBuffer.token)
				else
					raise "Error: #{result.to_s}"
				end
			end

			# Takes a token and gets the next token in the Negotiate authentication chain. Token can be Base64 encoded or not.
			# The token can include the "Negotiate" header and it will be stripped.
			# Does not indicate if SEC_I_CONTINUE or SEC_E_OK was returned.
			# Token returned is Base64 encoded w/ all new lines removed.
			def complete_authentication(token)
				raise "This object is no longer usable because its resources have been freed." if @cleaned_up

				# Nil token OK, just set it to empty string
				token = "" if token.nil?

				if token.include? "Negotiate"
					# If the Negotiate prefix is passed in, assume we are seeing "Negotiate <token>" and get the token.
					token = token.split(" ").last
				end

				if token.include? B64_TOKEN_PREFIX
					# indicates base64 encoded token
          token = token.strip.unpack("m")[0]
				end

				outputBuffer = SecurityBuffer.new
				result = SSPIResult.new(API::InitializeSecurityContext.call(@credentials.to_p, @context.to_p, nil,
					REQUEST_FLAGS, 0, SECURITY_NETWORK_DREP, SecurityBuffer.new(token).to_p, 0,
					@context.to_p,
					outputBuffer.to_p, @contextAttributes, TimeStamp.new.to_p))

				if result.ok? then
					return encode_token(outputBuffer.token)
				else
					raise "Error: #{result.to_s}"
				end
			ensure
				# need to make sure we don't clean up if we've already cleaned up.
				clean_up unless @cleaned_up
			end

		 private

			def clean_up
				# free structures allocated
				@cleaned_up = true
				API::FreeCredentialsHandle.call(@credentials.to_p)
				API::DeleteSecurityContext.call(@context.to_p)
				@context = nil
				@credentials = nil
				@contextAttributes = nil
			end

			# Gets credentials based on user, domain or both. If both are nil, an error occurs
			def get_credentials
				@credentials = CredHandle.new
				ts = TimeStamp.new
				@identity = Identity.new @user, @domain
				result = SSPIResult.new(API::AcquireCredentialsHandle.call(nil, "Negotiate", SECPKG_CRED_OUTBOUND, nil, @identity.to_p,
					nil, nil, @credentials.to_p, ts.to_p))
				raise "Error acquire credentials: #{result}" unless result.ok?
			end

			def encode_token(t)
				# encode64 will add newlines every 60 characters so we need to remove those.
        [t].pack("m").delete("\n")
			end
		end
	end
end