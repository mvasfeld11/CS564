SELECT COUNT(DISTINCT SellerID) AS num_sellers
FROM Items
JOIN EbayUsers ON Items.SellerID = EbayUsers.UserID
WHERE EbayUsers.Rating > 1000;
