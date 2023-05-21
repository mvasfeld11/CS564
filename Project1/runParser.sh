python turn_in_skeleton_parser.py ebay_data/items-*.json
sort -u Categories.dat > example.txt.tmp && mv example.txt.tmp Categories.dat
sort -u EbayUsers.dat > example.txt.tmp && mv example.txt.tmp EbayUsers.dat
sort -u Bids.dat > example.txt.tmp && mv example.txt.tmp Bids.dat
sort -u Items.dat > example.txt.tmp && mv example.txt.tmp Items.dat
sort -u AuctionData.dat > example.txt.tmp && mv example.txt.tmp AuctionData.dat
