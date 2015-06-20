/*
	INI File manager Include (myini.inc)
	* One of my oldest include! Now fast ini processing! Handling like DINI but faster.
	* This is just a fresh version! Added new features like supporting booleans +

 	Author: (creator)
	* Gammix

	Contributors:
	* [dRug]Slick - SII code

	(c) Copyright 2015
  	* This file is provided as is (no warranties).
*/

/*
FUNCTIONS:
native myini_exist(filename[]);
native myini_remove(filename[]);
native myini_rename(filename[], newname[]);
native myini_write(filename[], format[], {Float,_}:...);
native myini_read(filename[], format[], {Float,_}:...);
*/

//
#define myini_MAX_FIELDS 		50 //maximum number of lines or fields an ini file can have

#define myini_MAX_FIELD_NAME 	25 //maximum string size of a field's name

#define myini_MAX_FIELD_VALUE 	256 //maximum string size of field's value, can be a string, float, int or bool

#define myini_MAX_LINE_LENGTH 	(myini_MAX_FIELD_NAME + 1 + myini_MAX_FIELD_VALUE +  1 + 1) //maximum length of one field in a line (maybe you call it total string length of one line)

#define myini_MAX_FILE_NAME 	50 //maximum string size of a filename's name
//

enum CacheEnum
{
	E_KEY[myini_MAX_FIELD_NAME],
	E_VALUE[myini_MAX_FIELD_VALUE]
}
static gCache[myini_MAX_FIELDS][CacheEnum];
static gEmptyCache[CacheEnum];

enum FileEnum
{
	E_FILENAME[myini_MAX_FILE_NAME],
	bool:E_OPEN
}
static gFile[FileEnum];
static gNoFile[FileEnum];

//internal functions
stock static internal_myini_strcpy(dest[], source[], numcells = sizeof(source), maxlength = sizeof(dest))
{
	new i;
	while ((source[i]) && (i < numcells) && (i < maxlength))
	{
		dest[i] = source[i];
		i ++;
	}
	dest[(i == maxlength) ? (i - 1) : (i)] = '\0';
}

stock static internal_myini_stripnewline(string[])
{
	new l;
	l = strlen(string);
	if (string[l - 2] == '\r') string[l - 2] = '\0';
	if (string[l - 1] == '\n') string[l - 1] = '\0';
}

stock static internal_myini_open(filename[])
{
    if(! filename[0]) return false;
    if(gFile[E_OPEN])
	{
		if(strcmp(gFile[E_FILENAME], filename, true))
		{
			internal_myini_close();
		}
	}
	new File: h;
	h = fopen(filename, io_readwrite);
	if(h)
	{
		internal_myini_strcpy(gFile[E_FILENAME], filename, myini_MAX_FILE_NAME, myini_MAX_FILE_NAME);
		new line[myini_MAX_LINE_LENGTH];
		new ln = -1;
		new separator;
		while(((ln + 1) < myini_MAX_FIELDS) && (fread(h, line)))
		{
			ln ++;
			internal_myini_stripnewline(line);
			separator = strfind(line, "=", false);
			if((line[0] == ';') || (line[0] == '=') || (separator == -1) || (separator > myini_MAX_FIELD_NAME))
			{
				if(line[0] == ';')
				{
					internal_myini_strcpy(gCache[ln][E_VALUE], line, myini_MAX_FIELD_VALUE, myini_MAX_FIELD_VALUE);
				}
				else
				{
					internal_myini_strcpy(gCache[ln][E_VALUE][1], line, myini_MAX_FIELD_VALUE - 1, myini_MAX_FIELD_VALUE - 1);
					gCache[ln][E_VALUE][0] = ';';
				}
				continue;
			}
			internal_myini_strcpy(gCache[ln][E_KEY], line, separator, myini_MAX_FIELD_NAME);
			internal_myini_strcpy(gCache[ln][E_VALUE], line[separator + 1], myini_MAX_FIELD_VALUE, myini_MAX_FIELD_VALUE);
			if(! gCache[ln][E_VALUE][0]) gCache[ln][E_VALUE][0] = ' ';
		}
		fclose(h);
		gFile[E_OPEN] = true;
	}
	return true;
}

stock static internal_myini_close()
{
	new File: h;
	h = fopen(gFile[E_FILENAME], io_write);
	if(h)
	{
		new line[myini_MAX_LINE_LENGTH];
		new ln = -1;
		while(((ln + 1) < myini_MAX_FIELDS) && (gCache[ln + 1][E_VALUE][0]))
		{
			ln ++;
			if(gCache[ln][E_VALUE][0] == ';')
			{
				if(gCache[ln][E_VALUE][1])
				{
					format(line, sizeof(line), "%s\r\n", gCache[ln][E_VALUE]);
					fwrite(h, line);
					continue;
				}
				fwrite(h, "\r\n");
				continue;
			}
			format(line, sizeof(line), "%s=%s\r\n", gCache[ln][E_KEY], gCache[ln][E_VALUE]);
			fwrite(h, line);
		}
		fclose(h);
		return true;
	}

	for (new ln; ln < myini_MAX_FIELDS; ln++) gCache[ln] = gEmptyCache;
	gFile = gNoFile;
	return true;
}

stock static internal_myini_write(key[], value[])
{
	if(((! key[0]) || key[0] == ';') || (strfind(key, "=", false) != -1)) return false;
	new ln = -1;
	while(((ln + 1) < myini_MAX_FIELDS) && (gCache[ln + 1][E_VALUE][0]))
	{
		ln ++;
		if(gCache[ln][E_VALUE][0] == ';') continue;
		if(!strcmp(gCache[ln][E_KEY], key, false))
		{
			internal_myini_strcpy(gCache[ln][E_VALUE], value, myini_MAX_FIELD_VALUE, myini_MAX_FIELD_VALUE);
			return true;
		}
	}
	ln ++;
	if(ln < myini_MAX_FIELDS)
	{
		internal_myini_strcpy(gCache[ln][E_KEY], key, myini_MAX_FIELD_NAME, myini_MAX_FIELD_NAME);
		internal_myini_strcpy(gCache[ln][E_VALUE], value, myini_MAX_FIELD_VALUE, myini_MAX_FIELD_VALUE);
		return true;
	}
	return false;
}

stock static internal_myini_read(key[])
{
	new dest[myini_MAX_FIELD_VALUE];

	if(! key[0]) return dest;
	new ln = -1;
	while(((ln + 1) < myini_MAX_FIELDS) && (gCache[ln + 1][E_VALUE][0]))
	{
		ln ++;
		if(gCache[ln][E_VALUE][0] == ';') continue;
		if(!strcmp(gCache[ln][E_KEY], key, false))
		{
			internal_myini_strcpy(dest, gCache[ln][E_VALUE], myini_MAX_FIELD_VALUE, myini_MAX_FIELD_VALUE);
			return dest;
		}
	}
	return dest;
}

//main functions
stock myini_exist(filename[])
{
	return fexist(filename);
}

stock myini_remove(filename[])
{
	if(! filename[0]) return false;
	if((gFile[E_OPEN]) && (! strcmp(gFile[E_FILENAME], filename, true))) gFile = gNoFile;
	return fremove(filename);
}
#if defined _ALS_fremove
	#undef fremove
#else
	#define _ALS_fremove
#endif
#define fremove myini_remove

stock myini_rename(filename[], newname[])
{
	if(myini_exist(newname)) return false;

	new File: h, File: g, string[myini_MAX_FIELD_VALUE];
	h = fopen(filename, io_read);
	g = fopen(newname, io_write);

	if(!h) return false;
	if(!g) return (fclose(h));

	while(fread(h, string))
	{
		fwrite(g, string);
	}
	fclose(h);
	fclose(g);
	fremove(filename);
	return true;
}

stock myini_write(filename[], fmat[], {Float,_}:...)
{
    internal_myini_open(filename);

	new index = (0x-1);

    while(++index < strlen(fmat))
    {
        new length = (0x-1), loop = -1, param[0x80];

        while(++length < 0x80)
		{
			param[length] = getarg(2 + (index * 2), length);
        }
		switch(fmat[index])
        {
			case 'b':
			{
			    if(bool:getarg(3 + (index * 2)))
				{
				    internal_myini_write(param, "1");
			 	}
			 	else internal_myini_write(param, "0");
			}
			case 'd', 'i':
			{
			    new dest[12];
				format(dest, sizeof(dest), "%i", getarg(3 + (index * 2)));
				internal_myini_write(param, dest);
			}
			case 'f':
			{
			    new dest[12];
				format(dest, sizeof(dest), "%f", Float:getarg(3 + (index * 2)));
				internal_myini_write(param, dest);
			}
			case 's':
			{
			    new string[myini_MAX_FIELD_VALUE];
				while(++loop != 0x100)
				{
					string[loop] = getarg(3 + (index * 2), loop);
				}
				internal_myini_write(param, string);
 			}
		}
	}
	return internal_myini_close();
}

stock myini_read(filename[], fmat[], {Float,_}:...)
{
    internal_myini_open(filename);

    new index = (0x-1);

    while(++index < strlen(fmat))
    {
        new length = (0x-1), loop = (0x-1), param[0x80];

        while(++length < 0x80)
		{
			param[length] = getarg(2 + (index * 2), length);
        }
		switch(fmat[index])
	    {
	        case 'b':
			{
				if(strval(internal_myini_read(param)))
				{
					setarg(3 + (index * 2), 0, true);
				}
				else setarg(3 + (index * 2), 0, false);
			}
			case 'd', 'i':
			{
				setarg(3 + (index * 2), 0, strval(internal_myini_read(param)));
			}
			case 'f':
			{
				setarg(3 + (index * 2), 0, _:floatstr(internal_myini_read(param)));
			}
			case 's':
			{
			    new string[myini_MAX_FIELD_VALUE];
				string = internal_myini_read(param);

				while(strlen(string) != loop++)
				{
					setarg(3 + (index * 2), loop, string[loop]);
  				}
  			}
		}
    }
	return internal_myini_close();
}
