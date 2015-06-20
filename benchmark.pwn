#include <a_samp>
#include <myini>

public OnFilterScriptInit()
{
	new tick = GetTickCount();
	myini_write("test.ini",
						"iiiiisssssbbbbbfffff",
								"int1", 1, "int2", 1, "int3", 1, "int4", 1, "int5", 1,
								"str1", "string", "str2", "string", "str3", "string", "str4", "string", "str5", "string",
								"bool1", true, "bool2", true, "bool3", true, "bool4", true, "bool5", true,
								"float1", 0.0, "float2", 0.0, "float3", 0.0, "float4", 0.0, "float5", 0.0
				);
	printf("Time taken to write INI file: %i", GetTickCount() - tick);
	
	tick = GetTickCount();
	new value, valuestr[15], bool:valuebool, Float:valuefloat;
	myini_read("test.ini",
						"iiiiisssssbbbbbfffff",
								"int1", value, "int2", value, "int3", value, "int4", value, "int5", value,
								"str1", valuestr, "str2", valuestr, "str3", valuestr, "str4", valuestr, "str5", valuestr,
								"bool1", valuebool, "bool2", valuebool, "bool3", valuebool, "bool4", valuebool, "bool5", valuebool,
								"float1", valuefloat, "float2", valuefloat, "float3", valuefloat, "float4", valuefloat, "float5", valuefloat
				);
	printf("Time taken to read INI file: %i", GetTickCount() - tick);
}
