#!/bin/bash

#####################################################
# ANSI COLOUR
FG_RED="\033[1;31m"
FG_WHITE="\033[1m\033[37m"
FG_GRAY="\033[1m\033[37m"
FG_YELLOW="\033[1m\033[33m"
FG_GREEN="\033[1m\033[32m"
BG_BLUE="\033[1;44m"
txtrst='\e[0m'    # Text Reset

#--
#-- $1 title
#-- $2 message
warningMessage() {	echo -e "${FG_YELLOW}$1${txtrst} $2"; }
errorMessage()   {	echo -e "${FG_RED}$1${txtrst} $2"; }
infoMessage()    { 	echo -e "${FG_GREEN}$1${txtrst} $2"; }


#--
#-- Check if exist an index/type
#--
#-- Args:
#--  $1 = {host}/{_index}/{_type}
elasticsearch_exist()
{
	local _url="$1"
	#echo "--- Check if ${_url} exists"
	local http_rc=$(curl -XHEAD --write-out %{http_code} --silent --output /dev/null ${_url})
	if [ $http_rc -eq 200 ]; then
		return 0
	fi
	return 1
}

#--
#-- Uses Delete API to DELETE index/type/_all
#--
#-- Args:
#--  $1 = {host}/{_index}/{_type}
#--
#-- Globals
#--  $VERBOSE = Prints curl output to stdout
#--  $LOG_DIR = Log curl output on LOG_DIR
elasticsearch_delete()
{
	local _url="$1"

	local _verbose=0
	local _logfile="/dev/null"
	local _rc=1

	if [ -d "$LOG_DIR" ] && [ -w "$LOG_DIR" ]; then
		_logfile="${LOG_DIR}/`date +"%Y%m%d_%H%M%S"`_delete.log"
		test "$VERBOSE" = 1 && _verbose=1
	fi

	#-- ASK BEFORE DELETE
	warningMessage "[DELETE]" "${_url} ?\n"
	read -p "   Type 'Yes' to proceed: " choice

	if [ "$choice" = "Yes" ]; then
		_rc=`curl -XDELETE --write-out %{http_code} --output ${_logfile} --silent ${_url}?pretty=true`
	else
		echo "Operation Aborted by user" > $_logfile
	fi

	[ $_rc -ne 200 ] || [ $_verbose -eq 1 ] && cat $_logfile

	return $_rc
}

#--
#-- Uses Index API to Create an index
#--
#-- Args:
#--  $1 = {host}/{_index}/{_type}
#--  $2 = mapping file name
elasticsearch_create_index()
{
	local _url="$1"
	local _map="$2"

	local _verbose=0
	local _logfile="/dev/null"
	local _rc=1

	if [ -d "$LOG_DIR" ] && [ -w "$LOG_DIR" ]; then
		_logfile="${LOG_DIR}/`date +"%Y%m%d_%H%M%S"`_create_index.log"
		test "$VERBOSE" = 1 && _verbose=1
	fi

	local _curl_data=

	if elasticsearch_exist "${_url}" ; then
		warningMessage "[CREATE INDEX]" "${_url} Already Exist"
		return 1
	fi

	if [ ! -f "$_map" ]; then
		warningMessage "[CREATE INDEX]" "mapping file not found: ${_map}"
		echo -e "Continue without a mapping?\nType:"
		echo -e "  'Yes' to proceed"
		echo -e "  'No'  to abort operation"
		read -p "> " choice
		if [ "$choice" != "Yes" ]; then
			return 1
		else
			infoMessage "[CREATE INDEX]" "${_url} without mapping file"
		fi
	else
		infoMessage "[CREATE INDEX]" "${_url} using ${_map}"
		_curl_data="-d @${_map}"
	fi

	_rc=`curl -XPUT --write-out %{http_code} --output ${_logfile} --silent ${_url}?pretty=true ${_curl_data}`

	[ $_rc -ne 200 ] || [ $_verbose -eq 1 ] && cat $_logfile

	return $_rc
}

#--
#-- Get Mapping
#--
#-- Args:
#--  $1 = [{host}][/{_index}][/{_type}]
#--
#-- Globals
#--  $LOG_DIR = Log curl output on LOG_DIR, if not set log on <stdout>
elasticsearch_mapping()
{
	local _url="$1"
	local _logfile="-"
	local _rc=1

	infoMessage "[MAPPING]" "url:'${_url}'"
	if [ -z "$_url" ]; then
		errorMessage "[MAPPING]" "Invalid mapping url"
		return 1
	fi

	if [ -d "$LOG_DIR" ] && [ -w "$LOG_DIR" ]; then
		_logfile="${LOG_DIR}/`date +"%Y%m%d_%H%M%S"`_mapping.log"
	fi

	_rc=`curl -XGET --write-out %{http_code} --output ${_logfile} --silent "${_url}/_mapping?pretty=true"`

	[ $_rc -ne 200 ] || [ "$_logfile" != "-" ] && cat $_logfile

	return $_rc
}

#--
#-- Bulk API
#-- The endpoints are /_bulk, /{index}/_bulk, and {index}/{type}/_bulk
#--
#-- Args:
#--  $1 = {host}/{_index}/{_type}
#--  $2 = bulk data files
elasticsearch_bulk()
{
	local _url="$1"
	local _files=($2)	# array of filenames

	local _logfile="-"

	infoMessage "[BULK]" "Sending ${#_files[*]} bulk data files"
	for i in ${_files[*]}; do
		if [ ! -f "$i" ]; then
			warningMessage "[BULK]" "$i NOT FOUND!"
		else
			if [ -d "$LOG_DIR" ] && [ -w "$LOG_DIR" ]; then
				_logfile="${LOG_DIR}/`date +"%Y%m%d_%H%M%S"`_bulk_`basename $i`.log"
			fi
			infoMessage "[BULK]" "Sending ${i}"
			curl -XPOST --output ${_logfile} --silent "${_url}/_bulk?pretty=1" --data-binary "@$i"
		fi
	done
}
#-- ===========================================================================
#-- ===========================================================================

#-- ===========================================================================
create_temp_dir()
{
	#-- Create temporary dir for logging
	LOG_DIR=`mktemp -d`
	#-- trap signal to remove log_dir on exit
	trap "rm -rf $LOG_DIR" EXIT
}

setup_log_directory()
{
	local _logdir="$1"

	if [ -n "$_logdir" ] && [ -d "$_log_dir" ] && [ -w "$_logdir" ]; then
		#-- Ok $_logdir is a writable directory
		LOG_DIR="$_logdir"
		return 0
	fi

	if [ -z "$_logdir" ]; then
		#-- $_logdir is empty use a default temp dir
		create_temp_dir
		return 0
	fi

	if [ ! -d "$_logdir" ]; then
		#-- $_logdir is not a directory
		warningMessage "[LOGGING]" "'$_logdir' Is *NOT* a directory"
	elif [ ! -w "$_logdir" ]; then
		warningMessage "[LOGGING]" "You don't have write permissions on: '$_logdir'"
	else
		#-- $_logdir is empty use a default temp dir
		errorMessage "[LOGGING]" "Invalid directory: '$_logdir'"
	fi

	echo "Create a temporary dir for logging?"
	read -n 1 -p "[Press any key or 'CTRL+C' to abort]" dummy
	create_temp_dir
}

#-- ===========================================================================
show_help()
{
	#-- Using a	'here document' for help
	cat <<END_OF_HELP
-------------------------------------
Usage: create_index.sh [options | -h]"
  [Options]
	-H <host>  Set host (default: http://localhost:9200).
	-h         Show this help.
	-i <name>  Set {_index} name.
	-L <dir>   Use <dir> for logging (default tmp dir).
	-v         Set Verbose mode.

  [Create] options:
	-m <file> Set mapping from file

  [Bulk] options:
	-b "<file_1> <file_2> ... <file_n>"  Set bulk files
-------------------------------------
END_OF_HELP

}
#-- ===========================================================================
exec_command_show()
{
	case $_command in
		show_mapping) elasticsearch_mapping "${_host}/${_index}" ;;

		*) echo "'$_command' is an invalid command";
		   exit -1
		;;
	esac
}
#-- ===========================================================================
exec_command_delete()
{
	if elasticsearch_exist "${_host}/${_index}" ; then
		elasticsearch_delete "${_host}/${_index}"
		test $? -ne 200 && exit -1
	fi
}
#-- ===========================================================================
exec_command_create()
{
	elasticsearch_create_index "${_host}/${_index}" "$_mapping"
	test $? -ne 200 && exit -1
}
#-- ===========================================================================
exec_command_bulk()
{
	local _files=($_bulkdatafiles)

	if [ ${#_files[*]} -le 0 ]; then
		local _n=0
		local _input=
		local _done=
		echo "Enter filenames (blank line to end input)"

		while [ ! $_done ]; do
			read -p "Bulk file $_n: " _input
			test -z "$_input" && _done=1 || _files[$_n]=$_input
			_n=$[_n + 1]
		done

		echo "${#_files[*]} bulk data files"
		for i in ${_files[*]}; do
			echo $i
		done
	fi

	#-- The endpoints are /_bulk, /{index}/_bulk, and {index}/{type}/_bulk
	elasticsearch_bulk "${_host}" ${_files}
	#test $? -ne 200 && exit -1
}
#-- ===========================================================================



#-- Set Defaults
VERBOSE=0

_host="http://localhost:9200"
_index=
_mapping=
_bulkdatafiles=
_logdir=

while getopts b:c:H:i:m:L:hv opt; do
  case $opt in
  b) _bulkdatafiles=$OPTARG  ;;
  c) _command=$OPTARG  ;;
  H) _host=$OPTARG  ;;
  i) _index=$OPTARG ;;
  m) _mapping=$OPTARG ;;
  L) _logdir=$OPTARG ;;
  v) VERBOSE=1 ;;
  *) show_help
	 exit -1;;
  esac
done

shift $((OPTIND - 1))

#-- END PARSING COMMAND LINE

if [ -z "$_index" ]; then
	read -p "_index: " _index
	if [ -z "$_index"]; then
		echo "Cannot continue without an index name!"
		exit -1
	fi
fi

setup_log_directory "$_logdir"

echo "Command: $_command"
echo "Host   : $_host"
echo "Index  : $_index"

echo "Mapping: $_mapping"
echo "Bulks  : $_bulkdatafiles"

echo "Log dir: $LOG_DIR"
echo "Verbose: $VERBOSE"

case $_command in
	show_*) exec_command_show ;;
	delete) exec_command_delete ;;
	create) exec_command_create ;;
	  bulk) exec_command_bulk ;;
   	     *) echo "Invalid command: $_command"; exit -1 ;;
esac

read -n1 -p "Press any key to exit" dummy;
