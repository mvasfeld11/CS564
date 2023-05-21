SELECT COUNT(*) AS num_auctions
FROM (
    SELECT ItemID
    FROM Categories
    GROUP BY ItemID
    HAVING COUNT(DISTINCT Category_Name) = 4
) AS T;
