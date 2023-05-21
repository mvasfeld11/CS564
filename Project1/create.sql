/* Ensure that any old tables of the same name have been deleted */
drop table if exists Categories;
drop table if exists EbayUsers;
drop table if exists Bids;
drop table if exists Items;
drop table if exists AuctionData;

/* Create all of the necessary tables according to  schema design from Task B. */

create table Categories(
    ItemID INTEGER NOT NULL,
    Category_Name CHAR(300) NOT NULL,
    FOREIGN KEY (ItemID) REFERENCES Items(ItemID),
    PRIMARY KEY(ItemID, Category_Name)
);

create table EbayUsers(
    UserID CHAR(300),
    Rating INTEGER NOT NULL,
    Location CHAR(300),
    Country CHAR(300)
);

create table Bids(
    ItemID INTEGER NOT NULL,
    UserID CHAR(300) NOT NULL,
    TIME TIMESTAMP NOT NULL,
    Amount DECIMAL NOT NULL,
    FOREIGN KEY (ItemID) REFERENCES Items(ItemID),
    FOREIGN KEY (UserID) REFERENCES EbayUsers(UserID),
    PRIMARY KEY(ItemID, UserID, Amount)
);

create table Items(
    ItemID INTEGER NOT NULL PRIMARY KEY,
    SellerID CHAR(300) NOT NULL,
    Name CHAR(300) NOT NULL,
    Description TEXT NOT NULL,
    FOREIGN KEY (SellerID) REFERENCES EbayUsers(UserID)
);

create table AuctionData(
    AuctionItemID INTEGER NOT NULL PRIMARY KEY,
    First_Bid FLOAT,
    Started datetime NOT NULL,
    Currently FLOAT,
    Ends datetime NOT NULL,
    Number_Of_Bids INTEGER NOT NULL,
    Buy_Price FLOAT,
    FOREIGN KEY (AuctionItemID) REFERENCES Items(ItemID)
);
