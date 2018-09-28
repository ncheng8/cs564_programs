# -*- coding: cp1252 -*-

"""
FILE: skeleton_parser.py
------------------
Author: Firas Abuzaid (fabuzaid@stanford.edu)
Author: Perth Charernwattanagul (puch@stanford.edu)
Modified: 04/21/2014

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

    #Users
    usersDB = open("users.dat", "a") 
    #Items
    itemsDB = open("items.dat", "a") 
    #Bids
    bidsDB= open("bids.dat", "a")
    #Categories
    categoriesDB = open("categories.dat", "a")

    usersList = []

    with open(json_file, 'r') as f:
        items = loads(f.read())['Items'] # creates a Python dictionary of Items for the supplied json file
        for index, item in enumerate(items):
            
            """
            Item Info
            """
            itemID = item.get("ItemID")
            name = item.get("Name")
            category = item.get("Category")
            
            seller = item.get("Seller")
            sellerID = seller.get("UserID")
            sellerRating = seller.get("Rating")

            if sellerID not in usersList:
                usersList.append([sellerID, sellerRating, "", ""])
            
            currently = item.get("Currently")
            firstBid = transformDollar(item.get("First_Bid"))
            buyPrice = item.get("Buy_Price")
            numBids = transformDollar(item.get("Number_of_Bids"))
            startTime = transformDttm(item.get("Started"))
            ends = transformDttm(item.get("Ends"))
            description = item.get("Description")

            
            if itemID != None:
                itemsDB.write(itemID)

            if name != None:
                itemsDB.write("|" + name)

            if currently != None:
                itemsDB.write("|" + currently)

            if firstBid != None:
                itemsDB.write("|" + firstBid)

            if numBids != None:
                itemsDB.write("|" + numBids)

            if startTime != None:
                itemsDB.write("|" + startTime)

            if ends != None:
                itemsDB.write("|" + ends)

            if description != None:
                itemsDB.write("|" + description)

            if buyPrice != None:
                itemsDB.write("|" + buyPrice)

            if sellerID != None:
                itemsDB.write("|" + sellerID)

            itemsDB.write("\n")


            
            """
            Bids
            """
            
            bids = item.get("Bids")
            
            if (bids != None):
                for bid in bids:      
                        bidder = (bid.get("Bid").get("Bidder"))
                        
                        bidderID = bidder.get("UserID")
                        rating = bidder.get("Rating")
                        location = bidder.get("Location")
                        country = bidder.get("Country")

                        foundUser = False

                        for user in usersList:
                            if user[0] == bidderID:
                                if user[2] == "":
                                    user[2] = location
                                if user[3] == "":
                                    user[3] = country

                                foundUser = True

                        if foundUser == False:
                            usersList.append([bidderID, rating, location, country])
                                    
                        amount = (bid.get("Bid").get("Amount"))
                        time = (bid.get("Bid").get("Time"))

                        if bidderID != None:
                            bidsDB.write(bidderID)

                        if itemID != None:
                            bidsDB.write("|" + itemID)

                        if time != None:
                            bidsDB.write("|" + time)

                        if amount != None:
                            bidsDB.write("|" + amount)

                        itemsDB.write("\n")


    for user in usersList:
            
        if user[0] != None:
            usersDB.write(user[0])
            
        if user[1] != None:
            usersDB.write("|" + user[1])

        if user[2] != None:
            usersDB.write("|" + user[2])

        if user[3] != None:
            usersDB.write("|" + user[3])

        usersDB.write("\n")

    usersDB.close()
    itemsDB.close()
    bidsDB.close()
    categoriesDB.close()
                            
                    
            
"""
Loops through each json files provided on the command line and passes each file
to the parser
"""
def main(argv):
    if len(argv) < 2:
        print >> sys.stderr, 'Usage: python skeleton_json_parser.py <path to json files>'
        sys.exit(1)

    #Users
    usersDB = open("users.dat", "w") 
    #Items
    itemsDB = open("items.dat", "w") 
    #Bids
    bidsDB= open("bids.dat", "w")
    #Categories
    categoriesDB = open("categories.dat", "w")

    usersDB.close()
    itemsDB.close()
    bidsDB.close()
    categoriesDB.close()
        
    # loops over all .json files in the argument
    for f in argv[1:]:
        if isJson(f):
            parseJson(f)
            print ("Success parsing " + f)

if __name__ == '__main__':
    main(sys.argv)
