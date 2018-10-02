python our_parser.py ebay_data/items-*.json
sort categories.dat > cat.dat
uniq cat.dat > categories.dat
rm cat.dat