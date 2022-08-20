#!/bin/bash

local_directory=$(pwd)
n_ui_files=$(wc -l < $2)

rm $1.gresource.xml

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"		>> $1.gresource.xml
echo "<!-- Generated with generate-gresource-xml.sh  -->"	>> $1.gresource.xml
echo "<gresources>" 						>> $1.gresource.xml
echo "	<gresource prefix=\""$local_directory"\">"		>> $1.gresource.xml

while IFS= read -r line
do
	echo "		<file>"$line"</file>" 			>> $1.gresource.xml

	((iterator++))
done < $2

echo "	</gresource>"						>> $1.gresource.xml
echo "</gresources>"						>> $1.gresource.xml

