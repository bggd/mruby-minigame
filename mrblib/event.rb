module Minigame::Event
  @@type = 0

  class EVKeyboard
    
    attr_reader :keycode

    def initialize
      @keycode = 0
    end
  end

  @@keyboard = EVKeyboard.new

  class EVMouse

    attr_reader :x, :y, :z, :w, :dx, :dy, :dz, :dw, :button

    def initialize
      @x, @y = 0, 0
      @z, @w = 0, 0
      @dx, @dy = 0, 0
      @dz, @dw = 0, 0
      @button = 0
    end
  end

  @@mouse = EVMouse.new

  def self.type
    return @@type
  end

  def self.keyboard
    return @@keyboard
  end

  def self.mouse
    return @@mouse
  end

  def self.wait(&blk)
    loop do
      ev = Minigame::Event._wait_for_event()
      blk.call(ev)
    end
  end

  def self.poll(&blk)
    while ev = Minigame::Event._get_next_event()
      blk.call(ev)
    end
  end
end
