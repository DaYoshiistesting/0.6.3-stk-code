# run this script from the root directory to re-generate the .pot file
#
# ./data/po/update_pot.sh

CPP_FILE_LIST=`find ./src -name '*.cpp' -print`
PERL_FILE_LIST="`find ./data -name '*.track' -print` `find ./data -name '*.challenge' -print` `find ./data -name '*.grandprix' -print`"

echo "--------------------"
echo "    Source Files :"
echo "--------------------"
echo $CPP_FILE_LIST
echo "--------------------"
echo "    Data Files :"
echo "--------------------"
echo $PERL_FILE_LIST

echo "---------------------------"
echo "    Generating .pot file..."
xgettext    -d supertuxkart -s --keyword=_ --add-comments="I18N:" -p ./data/po -o supertuxkart.pot $CPP_FILE_LIST
xgettext -j -L perl -d supertuxkart -s --keyword=_ --add-comments="I18N:" -p ./data/po -o supertuxkart.pot $PERL_FILE_LIST
echo "    Done"
echo "---------------------------"