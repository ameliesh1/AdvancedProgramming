#!/bin/bash
######################################################################
# Name: junk.sh
# Author: Amelie Sharples aes2367
# Simple bash script that provides the functionality of a recycle bin.
# It moves files into the recycle directory and lists and purges
# files that have been placed in the recycle bin.
######################################################################

#readonly
readonly JUNK=".junk"

#flag variables
help_flag=0
list_flag=0
purge_flag=0

#here document
usage_message() {
cat <<-Endofmessage
Usage: '$0' [-hlp] [list of files]
	    -h: Display help.
	    -l: List junked files.
	    -p: Purge all files.
	    [list of files] with no other arguments to junk those files.
Endofmessage
}

while getopts ":hlp" option; do
    case "$option" in
        h) help_flag=1
           ;;
        l) list_flag=1
           ;;
        p) purge_flag=1
           ;;
        ?) printf "Error: Unknown option '-%s'.\n" "OPTARG" >&2
           usage_message
           exit 1
           ;;
    esac
done

#if too many flags or no flags or files
if [ $((help_flag + list_flag + purge_flag)) -gt 1 ]; then
    echo "Error: Too many options enabled."
    usage_message
    exit 1
elif [ $((help_flag + list_flag + purge_flag)) -eq 1 ]; then
    if [ $# -gt 1 ]; then
        echo "Error: Too many options enabled."
        usage_message
        exit 1
    fi
elif [ $((help_flag + list_flag + purge_flag)) -eq 0 ]; then
    if [ $# -eq 0 ]; then
        usage_message
        exit 1
    fi
fi

#if help_flag
if [ "$help_flag" -eq 1 ]; then
    usage_message
    exit 0
fi

#if junk directory doesn't exist
if [ ! -d "$HOME/$JUNK" ]; then
    mkdir $HOME/$JUNK
fi


#if list_flag
if [ "$list_flag" -eq 1 ]; then
    ls $HOME/$JUNK -lAF
    exit 0
fi


#if purge_flag
if [ "$purge_flag" -eq 1 ]; then
    pushd $HOME/$JUNK > /dev/null
    rm -rf .*  #delete all hidden files
    rm -rf * #delete all other files and directories
    popd > /dev/null
    exit 0
fi

#iterate through files, note those that DNE and remove the rest
for parameter in "$@"; do
    if [ -f "$parameter" ]; then
        mv "$parameter" "$HOME/$JUNK"
    elif [ -d "$parameter" ]; then
        mv "$parameter" "$HOME/$JUNK"
    elif [ ! -f "$parameter" ] || [ ! -d "$parameter" ]; then
        echo "Warning: '$parameter' not found"
    fi
done
