steamworks = {
	source = path.join(dependencies.basePath, "steamworks_sdk"),
}

function steamworks.import()
	links { "steam_api" }
	steamworks.includes()

	local redist_dir = path.translate(path.join(_WORKING_DIR, steamworks.source, "redistributable_bin", "steam_api.dll"), "\\")

	postbuildcommands {
		"copy /y \"" .. redist_dir .. "\" \"$(TargetDir)\""
	}
end

function steamworks.includes()
	includedirs {
		path.join(steamworks.source, "public")
	}

	libdirs {
		path.join(steamworks.source, "redistributable_bin")
	}
end

function steamworks.project()
end

table.insert(dependencies, steamworks)
