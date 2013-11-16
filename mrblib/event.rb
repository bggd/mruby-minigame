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
end
