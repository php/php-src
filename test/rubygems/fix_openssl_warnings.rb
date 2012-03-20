##
# HACK: this drives me BONKERS

if defined? OpenSSL then
  class OpenSSL::X509::ExtensionFactory
    alias :old_create_ext :create_ext
    def create_ext(*args)
      @config ||= nil
      old_create_ext(*args)
    end
  end
end if RUBY_VERSION < "1.9"
