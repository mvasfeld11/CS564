
"""
FILE: turn_in_skeleton_parser.py
------------------
Author: Oz Ogras (ogras@wisc.edu)
Author: Mark Vasfeld (vasfeld@wisc.edu)
Modified: 02/21/2023


Skeleton parser for CS564 programming project 1. Has useful imports and
functions for parsing, including:

1) Directory handling -- the parser takes a list of eBay json files
and opens each file inside of a loop. You just need to fill in the rest.
2) Dollar value conversions -- the json files store dollar value amounts in
a string like $3,453.23 -- we provide a function to convert it to a string
like XXXXX.xx.
3) Date/time conversions -- the json files store dates/ times in the form
Mon-DD-YY HH:MM:SS -- we wrote a function (transformDttm) that converts to the
for YYYY-MM-DD HH:MM:SS, which will sort chronologically in SQL.

Your job is to implement the parseJson function, which is invoked on each file by
the main function. We create the initial Python dictionary object of items for
you; the rest is up to you!
Happy parsing!
"""

import sys
from json import loads
from re import sub

columnSeparator = "|"

# Dictionary of months used for date transformation
MONTHS = {'Jan':'01','Feb':'02','Mar':'03','Apr':'04','May':'05','Jun':'06',\
        'Jul':'07','Aug':'08','Sep':'09','Oct':'10','Nov':'11','Dec':'12'}

"""
Returns true if a file ends in .json
"""
def isJson(f):
    return len(f) > 5 and f[-5:] == '.json'

"""
Converts month to a number, e.g. 'Dec' to '12'
"""
def transformMonth(mon):
    if mon in MONTHS:
        return MONTHS[mon]
    else:
        return mon

"""
Transforms a timestamp from Mon-DD-YY HH:MM:SS to YYYY-MM-DD HH:MM:SS
"""
def transformDttm(dttm):
    dttm = dttm.strip().split(' ')
    dt = dttm[0].split('-')
    date = '20' + dt[2] + '-'
    date += transformMonth(dt[0]) + '-' + dt[1]
    return date + ' ' + dttm[1]

"""
Transform a dollar value amount from a string like $3,453.23 to XXXXX.xx
"""

def transformDollar(money):
    if money == None or len(money) == 0:
        return money
    return sub(r'[^\d.]', '', money)
"""
Parses a single json file. Currently, there's a loop that iterates over each
item in the data set. Your job is to extend this functionality to create all
of the necessary SQL tables for your database.
"""
def parseJson(json_file):
    with open(json_file, 'r') as f:
        items = loads(f.read())['Items'] # creates a Python dictionary of Items for the supplied json file
        
        # Create .dat file for each entity we provided in our ER diagram
        file1 = open("Categories.dat", "a")
        file2 = open("Bids.dat", "a")
        file3 = open("EbayUsers.dat", "a")
        file4 = open("Items.dat", "a")
        file5 = open("AuctionData.dat","a")
        
        for item in items:
            """
            TODO: traverse the items dictionary to extract information from the
            given `json_file' and generate the necessary .dat files to generate
            the SQL tables based on your relation design
            """
            # print("item ", item)
            
            '''
            For future, can make it so there is a category_id that is not item_id
            '''
            # Get Categories data
            Categories = item["Category"]
            # A category to which the item belongs. An item may belong to multiple categories
            # print(Categories)
            
            # Get the itemID
            # Note that the ItemID attribute is unique and involved in only one auction, while the Name attribute is not unique.
            item_id = item['ItemID']
            # print(item_id)
            
            # check if the item_id is Null. If so then write "Null"
            if item_id is None:
                item_id = "Null"
            
            # traverse through the categories in the Categories dict
            for cat in Categories:
                data = item_id + "|" + '"' + cat.strip() + '"' + "\n"
                file1.write(data)
                
            
            # Get the Bids data
            bids = item['Bids']
            # print(item['Bids'])
            # There can be 'None' for Bids so initially check
            if bids is not None:
                for i in range(len(bids)):
                    bid_data = item["ItemID"]
                    bid = bids[i]['Bid']
                    # print("bid: ", bid)
                    # Check if the UserID is null
                    if bid['Bidder']['UserID'] is not None:
                        bid_data += '|' + '"' + bid['Bidder']['UserID'] + '"'
                    else:
                        # This means that it is None so write Null
                        bid_data += '|' + "Null"
                   
                    # transform time using given function. Also check if null
                    if bid["Time"] is not None:
                        bid_data += '|' + transformDttm(bid["Time"])
                    else:
                        bid_data += '|' + "Null"
                       
                    # Check Amount
                    if bid['Amount'] is not None:
                        bid_data += '|' + transformDollar(bid['Amount'])
                    else:
                        bid_data += '|' + "Null"
                    # add a new line to the end
                    bid_data += '\n'
                    file2.write(bid_data)
                    
            # In order to get the EbayUser data get the bidder's user info
            bid_info = item['Bids']
            if bid_info is not None:
                for i in range(len(bid_info)):
                    bidder = bid_info[i]["Bid"]["Bidder"]
                    # Check if null and append accordingly
                    if bidder['UserID'] is not None:
                        user_info = '"' + bidder['UserID'] + '"'
                    else:
                        user_info = "NULL"
                    
                    # Check if rating is null
                    if bidder['Rating'] is not None:
                        user_info += "|" + bidder["Rating"]
                    else:
                        user_info += "|" + "NULL"
                    
                    # Check if location is null
                    
                    if not "Location" in bidder.keys() or bidder["Location"] is None:
                        user_info += "|" + "NULL"
                    else:
                        # user_info += "|" + '"' + bidder['Location'] + '"'
                        user_info += "|" + '\"' + sub(r'\"','\"\"',bidder["Location"]) + '\"'
                    # Check if Country is null
                    if not "Country"  in bidder.keys() or bidder["Country"] == None:
                        user_info += "|" + "NULL"
                    else:
                        user_info += "|" + '"' + bidder['Country'] + '"'
                        
                    user_info += '\n'
                    file3.write(user_info)
                    
            # Get the seller info
            seller = item["Seller"]
            seller_id = seller['UserID']
            
            if seller_id is None:
                seller_id = "NULL"    
            else:
                seller_id = '"' + seller_id + '"'
            
            seller_info = seller_id
            if seller["Rating"] is not None:
                seller_info += "|" + seller["Rating"]
            else:
                seller_info += "|" + "NULL"
                
            if item['Location'] is not None:
                # seller_info += "|" + '"' + item['Location'] + '"'
                # print("item location: ", item['Location'])
                seller_info += "|" + '\"' + sub(r'\"','\"\"',item["Location"]) + '\"'
            else:
                seller_info += "|" + "NULL"
            
            if item['Country'] is not None:
                # seller_info += "|" + '"' + item['Country'] + '"' + "\n"
                seller_info += "|" + '"' + item["Country"].replace('"', '""') + '"\n'
            else:
                seller_info += "|" + "NULL" + "\n"
            
            file3.write(seller_info)
            
            # item_table(item)
            
             # Get Items data:

             #Taking in item_id and id of the seller immediately  
            items_data = "\"" + item_id + '\"' + "|" + seller_id 
            #Gets name of item
            if item["Name"] != None:
                items_data += "|" + '\"' + sub(r'\"','\"\"',item["Name"]) + '\"' 
            else:
                items_data += "|" + "NULL"
           
            #Gets description of the item
            if item["Description"] == None:
                items_data += "|" + "NULL" +"\n"
            else:
                items_data += "|" + '\"' + sub(r'\"','\"\"',item["Description"]) + '\"' +"\n"
            file4.write(items_data)
            
            
            # Get Auction_Block data:

            auction_item_ID = item["ItemID"]
            auction_data = auction_item_ID 
           
            #Gets the Buy_Price of the item in the auction
            if "Buy_Price" in item.keys():
                # print("The Buy_Price is: ", transformDollar(item["Buy_Price"]))
                auction_data += "|" + transformDollar(item["Buy_Price"])
            else:
                auction_data += "|" + "NULL"

            # Gets the first_bid value of the item in the auction
            if item["First_Bid"] is not None:
                # print("The First_Bid is: ", transformDollar(item["First_Bid"]))
                
                first_bid = transformDollar(item["First_Bid"])
                # auction_data += "|" + transformDollar(item["First_Bid"])
                
            else:
                first_bid = "NULL"
                # auction_data += "|" + "NULL"
           
            #Gets current price of item in auction
            if item["Currently"] is not None:
                # print("The Currently is: ", transformDollar(item["Currently"]))
                # auction_data +=  "|" + transformDollar(item["Currently"])
                currently = transformDollar(item["Currently"])
            else:
                currently = "NULL"
                
                # auction_data += "|" + "NULL"
            #Gets the number of bids on the item in the auction
            if item["Number_of_Bids"] is not None:
                
                num_of_bids = item["Number_of_Bids"]
                # print("The Number_of_Bids is: ", item['Number_of_Bids'])
                # auction_data += item["Number_of_Bids"]
            else:
                num_of_bids = "NULL"
                # auction_data += "NULL"
           
            #Gets the started time of item in the autction
            if item["Started"] is not None:
                started = transformDttm(item["Started"])
                # auction_data += "|" + transformDttm(item["Started"])
            else:
                started = "NULL"
                # auction_data += "|" + "NULL"           
            #Gets the end time if any, for the item in the auction
            if item["Ends"] is not None:
                # auction_data += "|" + transformDttm(item["Ends"])
                ends = transformDttm(item["Ends"])
            else:
                ends = "NULL"
                # auction_data += "|" + "NULL"
            
            auction_data += "|" + first_bid + "|" + currently
            auction_data += "|" + num_of_bids + "|" + started
            auction_data += "|" + ends
           
            auction_data += "\n"

            file5.write(auction_data)
            
            
        # close the files
        file1.close()
        file2.close()
        file3.close()
        file4.close()
        file5.close()

'''
Item = []
def item_table(item):
    
    global Item
    item_id = item['ItemID']
    name = item['Name'].replace('"', '""')
    seller = item["Seller"]
    seller_id = seller['UserID']
    
    if item['Description'] is not None:
        description = item['Description'].replace('"', '""')
    else:
        description = ""
        
    Item.append('"' + '"|"'.join([item_id, seller_id, name, description]) + '"\n')
    file4.write("".join(Item))
    
   # description = item['Description'].replace('"', '""') if item['Description'] is not None else ""
'''
    
"""
Loops through each json files provided on the command line and passes each file
to the parser
"""
def main(argv):
    if len(argv) < 2:
        print >> sys.stderr, 'Usage: python skeleton_json_parser.py <path to json files>'
        sys.exit(1)
    # loops over all .json files in the argument
    for f in argv[1:]:
        if isJson(f):
            parseJson(f)
            print("Success parsing " + f)

if __name__ == '__main__':
    main(sys.argv)
