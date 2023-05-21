SELECT AuctionItemID
FROM AuctionData
WHERE Currently = (SELECT MAX(Currently) FROM AuctionData);
