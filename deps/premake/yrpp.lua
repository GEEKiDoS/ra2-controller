yrpp = {
	source = path.join(dependencies.basePath, "yrpp"),
}

function yrpp.import()
	yrpp.includes()
end

function yrpp.includes()
	includedirs {
		yrpp.source
	}
end

function yrpp.project()
end

table.insert(dependencies, yrpp)
