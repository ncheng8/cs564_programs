DROP TABLE IF EXISTS Items;
DROP TABLE IF EXISTS Users;
DROP TABLE IF EXISTS Bids;
DROP TABLE IF EXISTS Categories;

CREATE TABLE Items (
	itemID INTEGER PRIMARY KEY,
	name TEXT,
	currently FLOAT,
	startBid FLOAT,
	numBids INTEGER,
	startTime TEXT,
	endTime TEXT,
	itemDesc TEXT,
	buyoutPrice FLOAT,
	sellerID TEXT, 
	FOREIGN KEY(sellerID) REFERENCES Users(userID)
);

CREATE TABLE Users (
	userID TEXT PRIMARY KEY,
	rating INTEGER,
	location TEXT,
	country TEXT	
);

CREATE TABLE Bids (
	bidderID TEXT PRIMARY KEY,
	itemID INTEGER,
	time TEXT,
	amount FLOAT,
	FOREIGN KEY(bidderID) REFERENCES Users(userID),
	FOREIGN KEY(itemID) REFERENCES Items(itemID)
);

CREATE TABLE Categories (
	itemID INTEGER,
	name TEXT,
	PRIMARY KEY(itemID, name),
	FOREIGN KEY(itemID) REFERENCES Items(itemID)	
);