SELECT COUNT(DISTINCT c.name)
FROM Categories c, (SELECT i.itemID idh
                   FROM Items i
                   WHERE i.currently > 100.00
                   AND i.numBids > 0) AS itemList

WHERE c.itemID = itemList.idh