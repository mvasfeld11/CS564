SELECT COUNT(DISTINCT Category_Name) AS num_categories
FROM Categories
WHERE ItemID IN (
    SELECT DISTINCT ItemID
    FROM Bids
    WHERE Amount > 100
);
