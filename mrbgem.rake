MRuby::Gem::Specification.new('mruby-minigame') do |spec|
  spec.license = 'MIT'
  spec.author  = 'bggd'

  case RUBY_PLATFORM
  when /linux/i
    spec.linker.libraries << %w(allegro allegro_main allegro_color allegro_image allegro_primitives allegro_audio allegro_acodec)
  end

  spec.bins = ['minigame']
end
