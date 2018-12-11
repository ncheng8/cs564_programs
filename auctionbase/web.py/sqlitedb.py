import web

db = web.database(dbn='sqlite',
        db='AuctionBase'
    )

######################BEGIN HELPER METHODS######################

# Enforce foreign key constraints
# WARNING: DO NOT REMOVE THIS!
def enforceForeignKey():
    db.query('PRAGMA foreign_keys = ON')

# initiates a transaction on the database
def transaction():
    return db.transaction()
# Sample usage (in auctionbase.py):
#
# t = sqlitedb.transaction()
# try:
#     sqlitedb.query('[FIRST QUERY STATEMENT]')
#     sqlitedb.query('[SECOND QUERY STATEMENT]')
# except Exception as e:
#     t.rollback()
#     print str(e)
# else:
#     t.commit()
#
# check out http://webpy.org/cookbook/transactions for examples

# returns the current time from your database
def getTime():
    query_string = 'select Time from CurrentTime;'
    results = query(query_string)
    return results[0].Time 

# returns a single item specified by the Item's ID in the database
# Note: if the `result' list is empty (i.e. there are no items for a
# a given ID), this will throw an Exception!
def getItemById(item_id):
    try:
        # TODO: rewrite this method to catch the Exception in case `result' is empty
        query_string = 'select * from Items where item_ID = $itemID'
        result = query(query_string, {'itemID': item_id})
        return result[0]
    except:
        return "Item not found."

# wrapper method around web.py's db.query method
# check out http://webpy.org/cookbook/query for more info
def query(query_string, vars = {}):
    return list(db.query(query_string, vars))

#####################END HELPER METHODS#####################

def setTime(time):
    query_kill = "delete from CurrentTime"
    db.query(query_kill)
    query_string = "insert into CurrentTime (Time) values ('" + time + "');"
    db.query(query_string)

def addBid(itemID, userID, amount):
    #Check that ItemID is valid
    query_itemID = 'select * from Items where ItemID = $itemID'
    itemIdResult = query(query_itemID, {'itemID': itemID})

    if(not itemIdResult):
        return False

    #Check that UserID is valid
    query_userID = 'select * from Users where UserID = $userID'
    userIdResult = query(query_userID, {'userID': userID})

    if(not userIdResult):
        return False

    #Check that amount > current amount
    myItem = itemIdResult[0]
    currently = myItem.Currently
    print("amount is " + str(amount) + " currently is " + str(currently))
    if(float(amount) < float(currently)):
        return False

    query_string = "insert into Bids (ItemID, UserID, Amount, Time) values ($item_id, $user_id, $amount, $time);"
    vars = {'item_id': str(itemID), 'user_id': str(userID), 'amount': str(amount), 'time': str(getTime())}
    #TODO ask why this is returning unicode error
    db.query(query_string, vars)
    return True

def searchAuctions(itemID, userID, minPrice, maxPrice, status):
    #TODO
    return
    
