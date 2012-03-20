module Rake

  # Manage several publishers as a single entity.
  class CompositePublisher
    def initialize
      @publishers = []
    end

    # Add a publisher to the composite.
    def add(pub)
      @publishers << pub
    end

    # Upload all the individual publishers.
    def upload
      @publishers.each { |p| p.upload }
    end
  end

end

