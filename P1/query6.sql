SELECT COUNT(*)
FROM Users u
WHERE u.userID IN (SELECT i.sellerID
                   FROM Items i)
AND u.userID IN (SELECT b.bidderID
                   FROM Bids b)