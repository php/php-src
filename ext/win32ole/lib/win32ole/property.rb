# OLEProperty
# helper class of Property with arguments.
class OLEProperty
  def initialize(obj, dispid, gettypes, settypes)
    @obj = obj
    @dispid = dispid
    @gettypes = gettypes
    @settypes = settypes
  end
  def [](*args)
    @obj._getproperty(@dispid, args, @gettypes)
  end
  def []=(*args)
    @obj._setproperty(@dispid, args, @settypes)
  end
end
