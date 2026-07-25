#!/usr/bin/env ruby
# Static XP package parser and SpeedTree asset cross-checker.

require "json"

def parse_xp(path, encrypted: false)
  data = File.binread(path)
  indicator, version, count = data.unpack("vvV")
  raise "bad XP header" unless indicator == 0xFEFA && version == 0xA001

  cursor = 8
  names = count.times.map do
    length = data.byteslice(cursor, 2).unpack1("v")
    cursor += 2
    name = data.byteslice(cursor, length).bytes.map { |byte| byte ^ 0xEF }.pack("C*")
    cursor += length
    name
  end
  offsets = data.byteslice(cursor, count * 4).unpack("V#{count}")

  entries = names.each_with_index.map do |name, index|
    finish = index + 1 < count ? offsets[index + 1] : data.bytesize
    payload = data.byteslice(offsets[index], finish - offsets[index])
    payload = payload.bytes.map { |byte| byte ^ 0xA5 }.pack("C*") if encrypted
    [name, payload]
  end
  { path: path, size: data.bytesize, count: count, metadata_end: cursor + count * 4, entries: entries }
end

def list_xp_names(path)
  File.open(path, "rb") do |file|
    indicator, version, count = file.read(8).unpack("vvV")
    return [] unless indicator == 0xFEFA && version == 0xA001
    count.times.map do
      length = file.read(2).unpack1("v")
      file.read(length).bytes.map { |byte| byte ^ 0xEF }.pack("C*")
    end
  end
rescue StandardError
  []
end

data_root = ARGV.fetch(0)
model = parse_xp(File.join(data_root, "Model", "MR_STMODEL.XP"))
texture = parse_xp(File.join(data_root, "Texture", "TR_SSTMODEL.XP"))
script = parse_xp(File.join(data_root, "Script", "SCR_BASE.XP"), encrypted: true)

scripts = script[:entries].to_h
tree_list = scripts.fetch(scripts.keys.find { |name| name.casecmp?("speedtreelist.ini") })
wind = scripts.fetch(scripts.keys.find { |name| name.casecmp?("speedwind.ini") })
lut = scripts.fetch(scripts.keys.find { |name| name.casecmp?("speedtreelut.ini") })

model_names = model[:entries].map(&:first)
texture_names = texture[:entries].map(&:first)
model_lookup = model_names.map(&:downcase).to_h { |name| [name, true] }
texture_lookup = texture_names.map(&:downcase).to_h { |name| [name, true] }

branch_refs = model[:entries].map do |model_name, payload|
  # In these legacy SPT payloads the first embedded TGA string is the branch
  # texture returned by STextures::m_pBranchTextureFilename. This ordering is
  # retained as an inference until verified through the matching SDK parser.
  [model_name, payload.scan(/[A-Za-z0-9_ .#\-]+\.tga/i).first]
end
affected_embedded_tgas = model[:entries].each_with_object({}) do |(model_name, payload), mapping|
  next unless model_name.match?(/PricklyPear|Saguaro|Azalea/i)
  mapping[model_name] = payload.scan(/[A-Za-z0-9_ .#\-]+\.tga/i).uniq
end
branch_unique = branch_refs.map(&:last).compact.uniq
branch_missing = branch_unique.reject { |name| texture_lookup[name.downcase] }
all_package_name_locations = {}
Dir.glob(File.join(data_root, "**", "*.[Xx][Pp]")).each do |package_path|
  list_xp_names(package_path).each do |entry_name|
    key = entry_name.downcase
    next unless branch_missing.any? { |missing| missing.downcase == key }
    (all_package_name_locations[entry_name] ||= []) << package_path.sub("#{data_root}/", "")
  end
end

parser_tokens = [
  "BranchStrengthExponent", "LeafStrengthExponent", "NumLeafAngles", "NumMatrices",
  "MaxBendAngle", "BendLowWind.p", "BendLowWind.i", "BendLowWind.d", "BendLowWind.a",
  "BendHighWind.p", "BendHighWind.i", "BendHighWind.d", "BendHighWind.a",
  "VibrationAngles.low", "VibrationAngles.high", "VibrationFrequency.low",
  "VibrationFrequency.high", "VibrationLowWind.p", "VibrationLowWind.i",
  "VibrationLowWind.d", "VibrationLowWind.a", "VibrationHighWind.p",
  "VibrationHighWind.i", "VibrationHighWind.d", "VibrationHighWind.a",
  "GustsPerMinute", "GustStrength.min", "GustStrength.max", "GustDuration.min",
  "GustDuration.max", "GustResponsiveness.p", "GustResponsiveness.i",
  "GustResponsiveness.d", "GustResponsiveness.a", "RockAngles.low",
  "RockAngles.high", "RockFrequency.low", "RockFrequency.high", "RockLowWind.p",
  "RockLowWind.i", "RockLowWind.d", "RockLowWind.a", "RockHighWind.p",
  "RockHighWind.i", "RockHighWind.d", "RockHighWind.a", "RustleAngles.low",
  "RustleAngles.high", "RustleFrequency.low", "RustleFrequency.high",
  "RustleLowWind.p", "RustleLowWind.i", "RustleLowWind.d", "RustleLowWind.a",
  "RustleHighWind.p", "RustleHighWind.i", "RustleHighWind.d", "RustleHighWind.a"
]
wind_lines = wind.lines.map(&:strip)
wind_pairs = wind_lines.reject { |line| line.empty? || line.start_with?("[") || line.start_with?("_XM_EOS") }
wind_keys = wind_pairs.map { |line| line.split("=", 2).first }
wind_bad_values = wind_pairs.reject do |line|
  _key, value = line.split("=", 2)
  value && value.strip.match?(/\A[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?\z/)
end

lines = tree_list.encode("UTF-8", invalid: :replace, undef: :replace).lines.map(&:strip)
meaningful = lines.reject { |line| line.empty? || line.start_with?("#", ";", "//") }
tokens = meaningful.map { |line| line.split(/\s+/) }

spt_refs = tree_list.scan(/[A-Za-z0-9_.\-]+\.spt/i).uniq
tga_refs = tree_list.scan(/[A-Za-z0-9_.\-]+\.tga/i).uniq
ref_lookup = spt_refs.map(&:downcase).to_h { |name| [name, true] }

result = {
  packages: {
    model: model.slice(:path, :size, :count, :metadata_end),
    texture: texture.slice(:path, :size, :count, :metadata_end),
    script: script.slice(:path, :size, :count, :metadata_end)
  },
  scripts: {
    speedtreelist_bytes: tree_list.bytesize,
    speedtreelist_lines: tree_list.lines.count,
    speedtreelist_meaningful_lines: meaningful.count,
    speedtreelist_token_widths: tokens.map(&:length).each_with_object(Hash.new(0)) { |width, counts| counts[width] += 1 },
    speedtreelist_preview: meaningful.first(8),
    speedwind_bytes: wind.bytesize,
    speedwind_lines: wind.lines.count,
    speedwind_ascii: wind.ascii_only?,
    speedwind_parameter_count: wind_pairs.count,
    speedwind_unique_parameter_count: wind_keys.uniq.count,
    speedwind_unrecognized_keys: wind_keys.uniq - parser_tokens,
    speedwind_missing_parser_keys: parser_tokens - wind_keys.uniq,
    speedwind_duplicate_keys: wind_keys.group_by(&:itself).select { |_key, values| values.count > 1 }.keys,
    speedwind_non_numeric_values: wind_bad_values,
    speedwind_preview: wind.lines.first(8).map(&:strip),
    speedtreelut_bytes: lut.bytesize,
    speedtreelut_lines: lut.lines.count
  },
  references: {
    spt_unique: spt_refs.count,
    spt_missing: spt_refs.reject { |name| model_lookup[name.downcase] },
    model_unreferenced: model_names.reject { |name| ref_lookup[name.downcase] },
    tga_unique: tga_refs.count,
    tga_missing: tga_refs.reject { |name| texture_lookup[name.downcase] },
    inferred_branch_reference_count: branch_refs.count { |_model, texture_name| texture_name },
    inferred_branch_unique: branch_unique.count,
    inferred_branch_missing: branch_missing,
    inferred_branch_found_in_other_packages: all_package_name_locations,
    inferred_branch_missing_models: branch_missing.each_with_object({}) do |texture_name, mapping|
      mapping[texture_name] = branch_refs.select { |_model, branch| branch == texture_name }.map(&:first)
    end,
    related_texture_names: texture_names.select { |name| name.match?(/rose|saguaro|azalea|cactus|stem|frond/i) },
    affected_model_embedded_tgas: affected_embedded_tgas,
    models_without_inferred_branch_reference: branch_refs.select { |_model, texture_name| texture_name.nil? }.map(&:first)
  },
  samples: {
    model_first: model_names.first(10),
    texture_first: texture_names.first(10)
  }
}

puts JSON.pretty_generate(result)
