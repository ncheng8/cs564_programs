SELECT i.itemID AS Auction
FROM (SELECT MAX(i.currently) AS maxCurr
      FROM Items i) AS a, Items i
WHERE i.currently = a.maxCurr